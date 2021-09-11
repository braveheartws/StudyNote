# GN快速入门指南

## 运行GN

您只需从命令行运行gn。对于大型项目，GN是版本控制的并与源代码签出一起分发。

* 对于Chromium和基于Chromium的项目，中有一个脚本depot_tools，可能在您的路径中，使用此名称。剧本将在包含当前目录和运行它。


* 对于树开发中的紫红色，请运行fx gn。。。哪个会找到合适的GN二进制文件，并使用给定的参数运行它。
* 有关其他项目，请参阅项目的文档。

## 建立一个构建

与其他一些构建系统不同，使用GN可以设置自己的构建目录使用您想要的设置。这使您可以在中维护尽可能多的不同版本你需要什么样的平行线。一旦你建立了一个构建目录，Ninja文件将被自动删除如果在该目录中生成时它们已过期，则重新生成不必重新运行GN。

创建生成目录的步骤

```
gn gen out/my_build
```

## 传递参数

通过运行以下命令在生成目录上设置生成参数：

```
gn args out/my_build
```

这将带来一个编辑。在该文件中键入build args，如下所示：

```
is_component_build = true 
is_debug = false
```

可用变量将取决于您的构建（此示例来自Chromium）。您可以看到可用参数及其默认值的列表通过打字

```
gn args --list out/my_build
```

在命令行上。请注意，您必须指定生成目录因为可用参数可以根据去建造。Chrome开发者也可以阅读特定于Chrome的构建配置有关更多信息，请参阅说明。

## 交叉编译到目标操作系统或体系结构

运行gn args out/Default（根据需要替换生成目录）并

为通用交叉编译选项添加以下一行或多行。

```
target_os = "chromeos" 
target_os = "android" 
target_cpu = "arm" 
target_cpu = "x86" 
target_cpu = "x64"
```

有关更多信息，请参阅GN交叉编译。

## Step-by-step

**添加生成文件**

转到目录`examples/simple_build`。这是最小GN的根存储库。在该目录中有一个教程目录。已经有一个`tutorial.cc`文件，该文件未连接到生成。创建一个新的`BUILD.gn`我们的新目标在该目录中的文件。

```
executable("tutorial") { 
    sources = [ 
    	"tutorial.cc", 
    ]
}
```

现在我们只需要告诉构建这个新目标。打开BUILD.gn父目录（`simple_build`）中的文件。GN从加载这个根开始文件，然后从这里向我们加载所有依赖项，所以我们只需要添加

此文件中对新目标的引用。

您可以将我们的新目标添加为中现有目标之一的依赖项` simple_build/BUILD.gn`文件，但它通常对将一个可执行文件作为另一个可执行文件的depdency（它们无法链接）。让我们组成一个“工具”小组。在GN中，“组”只是一个集合未遵守或链接的依赖项：

```
group("tools") { 
	deps = [ 
		# This will expand to the name "
		//tutorial:tutorial" which is the full name # of our new 			//target. Run "gn help labels" for more. 
		"//tutorial", 
		] 
}
```

## **Testing your addition**

从`simple_build`打开命令行

```
gn gen out 
ninja -C out tutorial 
out/tutorial
```

您应该看到“Hello，world.”输出到控制台。

旁注：GN鼓励使用非全局的静态库的目标名称独一无二。要构建其中一个，可以传递标签及其路径（但不传递前导）“/”）给 ninja：

```
ninja -C out some/path/to/target:my_target
```

## 声明依赖项

让我们看看中定义的目标示例/simple_build/build.gn。有一个定义一个函数`GetStaticText（）`的静态库：

```
static_library("hello_static") {
	sources = [ 
		"hello_static.cc", 
		"hello_static.h", 
	] 
}
```

还有一个共享库定义了一个函数`GetSharedText（）`：

```
shared_library("hello_shared") { 
	sources = [ 
		"hello_shared.cc", 
		"hello_shared.h", 
	]
	defines = [ "HELLO_SHARED_IMPLEMENTATION" ] 
}
```

这还说明了如何为目标设置预处理器定义。设置更多要分配多个值，请使用以下表格：

```
defines = [ 
	"HELLO_SHARED_IMPLEMENTATION", 
	"ENABLE_DOOM_MELON=0", 
]
```

现在让我们看看依赖于这两个库的可执行文件：

```
executable("hello") { 
	sources = [ 
		"hello.cc", 
	]
	
	deps = [ 
		":hello_shared", 
		":hello_static", 
	] 
}
```

此可执行文件包括一个源文件，并取决于上一个源文件两个`library`。以冒号开头的标签引用中具有该名称的目标当前BUILD.gn文件。

## 测试这个二进制文件

```
ninja -C out hello 
out/hello
```

请注意，您不需要重新运行GN。GN将自动重建当任何构建文件发生更改时，ninja文件将被删除。你知道会发生这种事当ninja打印[1/1]时，在开始时重新生成ninja文件执行

## 将设置放入配置中

库的用户通常需要编译器标志、定义和包含目录适用于他们。为此，请将所有此类设置放入一个“配置”中，该配置是命名的设置集合（但不是源或依赖项）：

```
config("my_lib_config") { 
	defines = [ 
		"ENABLE_DOOM_MELON" 
	] 
	include_dirs = [ 
		"//third_party/something" 
	] 
}
```

要将配置的设置应用于目标，请将其添加到`configs`列表：

```
static_library("hello_shared") {
	... 
	# Note "+=" here is usually required, see "default configs" below. 		configs += [ 
		":my_lib_config", 
	] 
}
```

配置可以应用于依赖于当前目标的所有目标，方法是其在`public_config`列表中的标签：

```
static_library("hello_shared") { 
	... 
    public_configs = [ 
        ":my_lib_config", 
    ] 
}
```

`public_configs`也适用于当前目标，因此不需要在两处列出一个配置。

## 默认配置

生成配置将设置一些设置，这些设置将通过违约这些通常会设置为默认配置列表。你可以看到这将使用“打印”命令执行，该命令对调试非常有用：

```
executable("hello") { 
	print(configs) 
}
```

运行GN将打印如下内容：

```
$ gn gen out 
["//build:compiler_defaults", "//build:executable_ldconfig"] 
Done. Made 5 targets from 5 files in 9ms
```

目标可以修改此列表以更改其默认值。例如，构建默认情况下，安装程序可能会通过添加` no_exceptions`配置来关闭异常，但目标可能会通过使用不同的目标来重新启用它们：

```
executable("hello") { 
    ... 
    configs -= [ "//build:no_exceptions" ] # Remove global default. 
    configs += [ "//build:exceptions" ] # Replace with a different one. 
}
```

上面的print命令也可以使用字符串插值来表示。这是一种将值转换为字符串的方法。它使用符号“$”来表示变量：

```
print("The configs for the target $target_name are $configs")
```

## 添加新的生成参数

您可以通过声明接受哪些参数并指定默认值`declare_args `

```
declare_args() { 
	enable_teleporter = true 
	enable_doom_melon = false 
}
```

有关如何工作的概述，请参阅`gn help buildargs`。

有关声明参数的详细信息，请参阅` gn help declare_args`。

在给定范围内多次声明给定参数是错误的，因此在范围界定和命名参数时应谨慎。



## 不知道发生了什么事？

您可以在详细模式下运行GN，以查看大量关于它是什么的消息做了什么。为此使用-v。

## “desc”命令

您可以运行` gn desc <build_dir> <targetname>`来获取有关给定目标：

```
gn desc out/Default //foo/bar:say_hello
```

将打印出许多令人兴奋的信息。您也可以只打印一个节。比如说你想知道你的两个人的定义来自`say_hello`目标：

```
> gn desc out/Default //foo/bar:say_hello defines --blame 
...lots of other stuff omitted... 
	From //foo/bar:hello_config 
		(Added by //foo/bar/BUILD.gn:12) 
	TWO_PEOPLE
```

另一个特别有趣的变化：

```
gn desc out/Default //base:base_i18n deps --tree
```

查看`gn help desc` 查看更多