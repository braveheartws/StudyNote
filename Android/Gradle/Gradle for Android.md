## Gradle for Android

```java
compileSdkVersion //用来编译应用的sdk版本

sourceSet 源集：一组源文件，他们会被一起执行和编译。对于Android而言main就是一个源集

MyApp
	|-build.gradle
	|-settings.gradle	定义了哪些模块应该包含在构建内
	|-app
		|-build.gradle

project目录 build.gradle
buildscript {	//Gradle 和android插件所在的仓库
    ext.kotlin_version = "1.3.72"
    repositories {
        google()
        jcenter()
    }
    dependencies {
        classpath "com.android.tools.build:gradle:4.0.0"
        classpath "org.jetbrains.kotlin:kotlin-gradle-plugin:$kotlin_version"
    }
}
allprojects {	//声明哪些被用于所有模块的属性
    repositories {
        google()	//从这些仓库下载代码
        jcenter()
    }
}
app目录 build.gradle
apply plugin: 'com.android.application'	//插件

android {
    compileSdkVersion 30	//用来编译应用的AndroidAPI版本
    buildToolsVersion "30.0.3"	//构建工具和编译器使用的版本号

    defaultConfig {
        applicationId "com.example.sample"	//覆盖了manifest里面的packagename，（manifest packagename作用： 应用唯一标志以及在R资源类中被用作包名，）
        minSdkVersion 21	//运行应用最小api级别
        targetSdkVersion 30	//用于通知系统，该应用已在某特定的android版本通过测试，操作系统不必启用任何向前兼容的行为，和compileSdkVersion没有任何关系
        versionCode 1		//版本号
        versionName "1.0"	//版本名称

        testInstrumentationRunner "androidx.test.runner.AndroidJUnitRunner"
    }

    buildTypes {	//用来定义如何构建和打包不同构建类型的应用
        release {
            minifyEnabled false
            proguardFiles getDefaultProguardFile('proguard-android-optimize.txt'), 'proguard-rules.pro'
        }
    }
}

dependencies {	//依赖包
    implementation fileTree(dir: "libs", include: ["*.jar"])	//libs目录下所有以.jar结尾的包
    implementation "org.jetbrains.kotlin:kotlin-stdlib:$kotlin_version"
}

gradlew tasks //打印所有可用的任务
gradlew tasks --all //查看全部的任务依赖关系

gradle 基础任务
assemble: 集合项目的输出
clean: 清理项目的输出
check: 运行所有检查，通常是单元测试和集成测试
build: 同时运行assemble和check

gradle android任务
assemble: 为每个构建版本创建一个apk
clean:	删除所有的构建内容
check:	运行Lint检查，如果发现一个问题则终止构建
build:	同时运行assemble和check
connectedCheck: 在连接模拟器或者设备上进行测试
deviceCheck:	一个占位任务，专为其他插件在远端设备上运行测试
installDebug 和 installRelease: 在连接设备上安装特定版本
installTasks 都会有相关的 uninstall任务

gradle允许在project对象上添加额外属性，这意味着任何build.gradle文件都能定义额外属性，添加额外属性需要通过ext代码块
可以给顶层文件添加含有自定义属性的 ext 代码块
project目录 build.gradle
ext {
	compileSdkVersion 30
    buildToolsVersion "30.0.3"
}
使用：
android {
	compileSdkVersion rootProject.ext.compileSdkVersion
    buildToolsVersion rootProject.ext.buildToolsVersion
}

定义属性的常用方式：
1.ext代码块
2.gradle.properties文件
3.-p命令参数

依赖元素
group：name：version		依赖库组织(反向域名):依赖库唯一标识:版本号
com.google.code.gson:gson:2.3	//

远程仓库
repositories {
	maven {
		url "http://repo.acmecorp.com/repo"	放在自有的服务器上
		credentials {									如果需要添加校验的话，但不建议这种方式，构建配置文件是纯文本，会被迁入源码控制系统，更好的方法是单独的Gradle属性文件
			username 'junpuzhang3012@gmail.com'
			password 'pwdxxxx'
		}
	}
}
本地仓库
repositories {
	maven {
		url "C:\\Users\\Administrator\\Desktop\\pgc"	绝对路径
	}
	flatDir {
		dirs 'aars'	能够让dependence模块使用文件夹下的依赖
	}
}
本地依赖
文件依赖
dependencies {
	compile files('libs/xxx.jar')	//单个jar依赖
	compile fileTree('libs')		//完整的文件夹依赖
	compile fileTree(dir: 'libs', include:[*.jar])	//增加一个过滤，只有.jar文件结尾的才被添加， 这个语法有点像kt的参数声明
}
原生依赖库
app
|-jniLibs
	|-areabi
		|-nativelib.so
	|-x86
		|-nativelib.so	
android {
	sourceSet.main {
		jniLibs.srcDir 'src/main/libs'
	}
}
依赖项目
创建项目模块： apply plugin: 'com.android.library'
dependencies {
	compile project(':library')
}
使用.aar
allprojects {
	repositories {
		flatDir {
			dirs 'D:\\code\\demo\\Sample\\mylibrary\\build\\outputs\\aar'
		}
	}
}
dependencies {
	implementation(name:'mylibrary-debug',ext:'aar')  可以添加任何目录到文件夹，ext告知Gradle查找具有特定名称且拓展名为.aar的依赖库
}

依赖概念
配置：	比如某些定制的sdk运行在特定的设备上，但是需要定制sdk添加进参与编译路径，但并不需要将sdk添加到apk，这便是依赖配置
依赖库标准配置
compile
implementation
api
apk
provided
testCompile
androidTestCompile
.....
语义化版本
major.minor.patch
当做不兼容API变化时 major增加
当向后兼容的方式添加功能时 minor增加
当修复一些bug时 patch增加

动态化版本
在某些情况下我们希望每次构建应用依赖时，都能获取最新依赖。要做到这一点就需要动态化版本。
dependencies {
	compile 'com.android.support:support-v4:22.2.+'		告知Gradle获取最新patch版本
	compile 'com.android.support:appcompat-v7:22.2+'	告知Gradle获取minor最新版本，且minor版本至少是2
	compile 'com.android.support:recycleview:+'			告知Gradle获取最新版本
}
备注：使用动态化版本需要额外注意，可能Gradle获取的依赖版本并不稳定，或者机器上运行着不同版本依赖与构建服务器的版本不一致，导致崩溃或者呈现与预期不一致

-----------------------------------------不懂--------------------------------
flavorDimensions 'api','version'
productFlavors {
    demo {
        dimension 'api'
    }
    full {
        dimension 'version'
    }
    minApi21 {
        dimension "api"
        minSdkVersion '21'
        versionNameSuffix "-minApi21"
    }
}
------------------------------------------------------------------------------
任务
一个新的Android应用默认用release和debug两种构建类型，可以用assembleDubug or  assembleRelase 构建两种类型，也可以执行 assemble同时构建两个apks
签名配置
	signingConfigs {
        staging.initWith(signingConfigs.debug)
        release {
            storeFile file("release.ketsotre")
            storePassword ""
            keyAlias ""
            keyPassword ""
        }
    }

    buildTypes {
        release {
            signingConfig signingConfigs.release 	//use
        }
    }

    productFlavors {
        demo {
            dimension 'api'
            signingConfig signingConfigs.release 	//use
        }
    }
构建的生命周期   初始化阶段 -> 配置阶段 -> 执行阶段
settings.gradle ->  

只构建某一个模块：gradlew :app:assembleDebug

gradle.properties 
org.gradle.parallel=true //并行构建
org.gradle.daemon=true //守护进程，构建时服用守护进程，减少启动脚本 一般默认开启
org.gradle.jvmargs=-Xms256m -Xmx1024m	Xms:设置初始内存大小 Xmx:最大内存
org.gradle.configureondemand=true	//多模块非常有用:gradle在运行配置阶段前指出哪一个模块有配置改变，哪一个没有。


任务：
task hello {
    println 'wo shi  kehan'
}

tasks.create(name: 'hello2') {
    println 'hello'
}

task(nihao) {
    println 'nihao'
}
>>gradlew hello

android.applicationVariants.all { variant ->
    variant.outputs.each { output ->
        def file = output.outputFile
        outputFile = new File(file.parent,file.name.replace('.apk',"-${variant.versionName}.apk"))
    }
}

执行命令行
task testCommand {
	doLast {
        exec {
            executable('adb')
            args = ['shell','dumpsys','SurfaceFlinger']
        }
    }
}
插件开发
step1:
class MyPlugin implements Plugin<Project> {
	void apply(Project project) {

	}
}
step2:
apply plugin:MyPlugin

----------------------------------------------------------------------------------------------------------------------
分发插件：
step1 
创建build.gradle
		apply plugin: 'groovy'

		dependencies {
			compile gradleApi()
			compile localGroovy()
		}
step2
保证目录结构
plugin
	|--src
		|--main
			|--groovy
				|--com
					|--packagename
						|--name
			resources
				|--META-INF
					|--gradle-plugins
						|--com.gradleforandroid.run.properties //.properties 是文件名  里面写一句话 implementation-class=com.gradleforandroid.MyPlugin

step3:
发布
apply plugin: 'maven'
uploadArchives {
	repositories {
		mavenDeployer {
			repository(url: uri('repo_url'))
		}
	}
}

step4:
使用插件

----------------------------------------------------------------------------------------------------------------------
gradlew assembleDebug --profile 显示输出构建耗时的详情(html)
```

