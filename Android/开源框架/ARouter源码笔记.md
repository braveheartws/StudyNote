### 一.基础使用

github地址[alibaba/ARouter](https://github.com/alibaba/ARouter)

1. 添加依赖

   ```groovy
   android {
       defaultConfig {
           ...
           javaCompileOptions {
               annotationProcessorOptions {
                   arguments = [AROUTER_MODULE_NAME: project.getName()]
               }
           }
       }
   }
   
   dependencies {
       // Replace with the latest version
       compile 'com.alibaba:arouter-api:1.5.2'
       annotationProcessor 'com.alibaba:arouter-compiler:1.5.2'
   }
   ```

2. 在activity上添加注解

   ```java
   @Route(path = "/test/activity")
   public class YourActivity extend Activity {
       ...
   }
   ```

3. 跳转

   ```java
   ARouter.getInstance().build("/test/1")
               .withLong("key1", 666L)
               .withString("key3", "888")
               .withObject("key4", new Test("Jack", "Rose"))
               .navigation();
   ```

---

### 二.代码分析

#### 2.1初始化

```java
ARouter.init(this);	// Application初始化
```

```java
public static void init(Application application) {
        if (!hasInit) {
            ...
            hasInit = _ARouter.init(application);
			...
        }
}
```

```java
protected static synchronized boolean init(Application application) {
        mContext = application;
        LogisticsCenter.init(mContext, executor);
        ...
        return true;
}
```

```java

```



#### 2.2构建Postcard

```java
Postcard postcard = ARouter.getInstance().build(String);
```

#### 2.3navigation

```java
ARouter.getInstance().build(String).navigation();
	return _ARouter.getInstance().navigation(mContext, postcard, requestCode, callback);
		LogisticsCenter.completion(postcard);// 这个里面并不会真正的跳转,只是赋值了一些参数
			RouteMeta routeMeta = Warehouse.routes.get(postcard.getPath());
			postcard.setType(routeMeta.getType());	// 设置类型
			Map<String, Integer> paramsType = routeMeta.getParamsType(); // 获取参数
		return _navigation(postcard, requestCode, callback);
			final Intent intent = new Intent(currentContext, postcard.getDestination());
			if (!(currentContext instanceof Activity)) {	// 当传入的context不是activitycontext
            	intent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
            }
			startActivity(requestCode, currentContext, intent, postcard, callback);
	
```

---

### 三.ARouter的注解处理器

#### 3.1 AutowiredProcessor

> 自动注入参数的注解处理器

```java
//AutowiredProcessor 自动生成的类

public class Main2Activity$$ARouter$$Autowired implements ISyringe {
  private SerializationService serializationService;

  @Override
  public void inject(Object target) {
    serializationService = ARouter.getInstance().navigation(SerializationService.class);
    Main2Activity substitute = (Main2Activity)target;
    substitute.mName = substitute.getIntent().getExtras() == null ? substitute.mName : substitute.getIntent().getExtras().getString("mName", substitute.mName);
    substitute.mAge = substitute.getIntent().getIntExtra("mAge", substitute.mAge);
  }
}
```



#### 3.2 RouteProcessor

> 将有@Route这样注解的类生成辅助类添加进集合

```java
public class ARouter$$Group$$app implements IRouteGroup {
  @Override
  public void loadInto(Map<String, RouteMeta> atlas) {
    atlas.put("/app/activity", RouteMeta.build(RouteType.ACTIVITY, MainActivity.class, "/app/activity", "app", null, -1, -2147483648));
    atlas.put("/app/activity2", RouteMeta.build(RouteType.ACTIVITY, Main2Activity.class, "/app/activity2", "app", new java.util.HashMap<String, Integer>(){{put("mAge", 3); put("mName", 8); }}, -1, -2147483648));
  }
}

public class ARouter$$Root$$app implements IRouteRoot {
  @Override
  public void loadInto(Map<String, Class<? extends IRouteGroup>> routes) {
    routes.put("app", ARouter$$Group$$app.class);
  }
}
```

#### 3.3 参数注入

```java
AutowiredService autowiredService = ((AutowiredService) ARouter.getInstance().build("/arouter/service/autowired").navigation());
autowiredService.autowire(thiz);
    doInject(instance, null);
    doInject(instance, superClazz);
		ISyringe syringe = getSyringe(clazz);
			syringeHelper = (ISyringe) Class.forName(clazz.getName() + SUFFIX_AUTOWIRED).getConstructor().newInstance();	// 创建  Main2Activity$$ARouter$$Autowired 实例
		syringe.inject(instance);// 调用到inject@Main2Activity$$ARouter$$Autowired自动注入
	
		
```

#### 3.4 PluginLaunch插件

```groovy
public void apply(Project project) {
    def android = project.extensions.getByType(AppExtension)
    def transformImpl = new RegisterTransform(project)
    RegisterTransform.registerList = list
    ...
    android.registerTransform(transformImpl)	// 将自己的transform注册
}


registerList.each { ext ->
	RegisterCodeGenerator.insertInitCodeTo(ext)
}

static void insertInitCodeTo(ScanSetting registerSetting) {
        if (registerSetting != null && !registerSetting.classList.isEmpty()) {
            RegisterCodeGenerator processor = new RegisterCodeGenerator(registerSetting)
            File file = RegisterTransform.fileContainsInitClass
            if (file.getName().endsWith('.jar'))
                processor.insertInitCodeIntoJarFile(file)	// 插入初始化代码到jar文件
        }
}

//访问方法
private File insertInitCodeIntoJarFile(File jarFile) {
    ...
    def bytes = referHackWhenInit(inputStream)
    	ClassVisitor cv = new MyClassVisitor(Opcodes.ASM5, cw)
    		mv = new RouteMethodVisitor(Opcodes.ASM5, mv)
    			mv.visitLdcInsn(name)//类名
    			mv.visitMethodInsn(Opcodes.INVOKESTATIC
                            , ScanSetting.GENERATE_TO_CLASS_NAME	//com/alibaba/android/arouter/core/LogisticsCenter
                            , ScanSetting.REGISTER_METHOD_NAME	//register
                            , "(Ljava/lang/String;)V"
                            , false)
    ...
}

//LogisticsCenter
private static void register(String className) {
        if (!TextUtils.isEmpty(className)) {
            try {
                Class<?> clazz = Class.forName(className);
                Object obj = clazz.getConstructor().newInstance();
                if (obj instanceof IRouteRoot) {
                    registerRouteRoot((IRouteRoot) obj);
                } else if (obj instanceof IProviderGroup) {
                    registerProvider((IProviderGroup) obj);
                } else if (obj instanceof IInterceptorGroup) {
                    registerInterceptor((IInterceptorGroup) obj);
                } else {
                    logger.info(TAG, "register failed, class name: " + className
                            + " should implements one of IRouteRoot/IProviderGroup/IInterceptorGroup.");
                }
            } catch (Exception e) {
                logger.error(TAG,"register class error:" + className, e);
            }
        }
}

private static void registerRouteRoot(IRouteRoot routeRoot) {
        markRegisteredByPlugin();
        if (routeRoot != null) {
            routeRoot.loadInto(Warehouse.groupsIndex);
        }
}
// 最终给Warehouse里面的routes, groupsIndex 赋值
// static Map<String, RouteMeta> routes = new HashMap<>();
// static Map<String, Class<? extends IRouteGroup>> groupsIndex = new HashMap<>();
public class ARouter$$Root$$app implements IRouteRoot {
  @Override
  public void loadInto(Map<String, Class<? extends IRouteGroup>> routes) {
    routes.put("app", ARouter$$Group$$app.class);
  }
}
```

##### 3.3.1 插件修改后的class文件

> 会自动将扫描到路由信息添加到集合

```java
// 修改前的方法
private static void loadRouterMap() {
    registerByPlugin = false;
    // auto generate register code by gradle plugin: arouter-auto-register
    // looks like below:
    // registerRouteRoot(new ARouter..Root..modulejava());
    // registerRouteRoot(new ARouter..Root..modulekotlin());
}

// 经过gradle插件操作后
private static void loadRouterMap() {
    registerByPlugin = false;
    register("com.alibaba.android.arouter.routes.ARouter$$Root$$arouterapi");
    register("com.alibaba.android.arouter.routes.ARouter$$Root$$shortcode");
    register("com.alibaba.android.arouter.routes.ARouter$$Root$$app");
    register("com.alibaba.android.arouter.routes.ARouter$$Providers$$arouterapi");
    register("com.alibaba.android.arouter.routes.ARouter$$Providers$$shortcode");
    register("com.alibaba.android.arouter.routes.ARouter$$Providers$$app");
}
```

#### 3.4 SP缓存问题

```xml
<map>
    <set name="ROUTER_MAP">
        <string>com.alibaba.android.arouter.routes.ARouter$$Group$$app</string>
        <string>com.alibaba.android.arouter.routes.ARouter$$Group$$shortcode</string>
        <string>com.alibaba.android.arouter.routes.ARouter$$Root$$arouterapi</string>
        <string>com.alibaba.android.arouter.routes.ARouter$$Group$$app$1</string>
        <string>com.alibaba.android.arouter.routes.ARouter$$Providers$$arouterapi</string>
        <string>com.alibaba.android.arouter.routes.ARouter$$Root$$shortcode</string>
        <string>com.alibaba.android.arouter.routes.ARouter$$Group$$thirdparty</string>
        <string>com.alibaba.android.arouter.routes.ARouter$$Providers$$app</string>
        <string>com.alibaba.android.arouter.routes.ARouter$$Providers$$shortcode</string>
        <string>com.alibaba.android.arouter.routes.ARouter$$Group$$arouter</string>
        <string>com.alibaba.android.arouter.routes.ARouter$$Root$$app</string>
    </set>
    <string name="LAST_VERSION_NAME">1.0</string>
    <int name="LAST_VERSION_CODE" value="1" />
</map>
```



---

### 四.遇到问题

#### 4.1 模块间不能跳转

app模块跳转到shortcode模块不能跳转

原因: shortcode模块依赖common模块,先前只在common添加了注解处理器的依赖`annotationProcessor 'com.alibaba:arouter-compiler:1.5.2'`

解决: 在每个模块都应该添加注解处理器的依赖`annotationProcessor 'com.alibaba:arouter-compiler:1.5.2'`,同时可以看[app/build/generated/ap_generated_sources/debug/out]下面有没有生成`ARouter$$Group$$app`这样的辅助类

