## Jetpack分析

### 一 Lifecycle

#### 1.1 使用

1.P实现LifecycleObserver接口, 给函数加上注解

```java
public class P implements LifecycleObserver {
    @OnLifecycleEvent(Lifecycle.Event.ON_CREATE)
    public void onCreate() {
        Log.d(TAG, "onCreate: ");
    }
}
```

2.Activity中注册监听

```java
protected void onCreate(Bundle savedInstanceState) {
	getLifecycle().addObserver(presenter);
}
```

#### 1.2 实现

1.2.1 `getLifecycle()`分析

`getLifecycle()`返回了`ComponentActivity`的`mLifecycleRegistry`

```java
private final LifecycleRegistry mLifecycleRegistry = new LifecycleRegistry(this);
```

12.1 `addObserver`方法

```java
// INITIALIZED为初始状态
State initialState = mState == DESTROYED ? DESTROYED : INITIALIZED;
ObserverWithState statefulObserver = new ObserverWithState(observer, initialState);
	mLifecycleObserver = Lifecycling.lifecycleEventObserver(observer);
		int type = getObserverConstructorType(klass);
			int type = resolveObserverCallbackType(klass); // return REFLECTIVE_CALLBACK 1
				boolean hasLifecycleMethods = 			ClassesInfoCache.sInstance.hasLifecycleMethods(klass);	// true
		return new ReflectiveGenericLifecycleObserver(object);
State targetState = calculateTargetState(observer);	// 计算目标状态
// upEvent(statefulObserver.mState) 会从INITIALIZED -> ON_CREATE
statefulObserver.dispatchEvent(lifecycleOwner, upEvent(statefulObserver.mState));
	// 获取即将要执行的状态
	State newState = getStateAfter(event);
	// 这里 mLifecycleObserver = ReflectiveGenericLifecycleObserver
	// mWrapped = MainPresenter, source = MainActivity, event要执行的生命周期
	mLifecycleObserver.onStateChanged(owner, event);
		mInfo.invokeCallbacks(source, event, mWrapped);
			// 从mEventToHandlers里面拿到需要执行的方法
			invokeMethodsForEvent(mEventToHandlers.get(event), source, event, target);
				handlers.get(i).invokeCallback(source, event, mWrapped);
					// 通过反射调用了方法
					mMethod.invoke(target);
	
```

**如何收集含有注解的方法?**

在`addObserver`方法调用的时候,初次会调用`ClassesInfoCache.sInstance.hasLifecycleMethods(klass)`方法,这里会通过反射遍历所有方法查看是否有`OnLifecycleEvent`这个注解,随后会调用`createInfo`里面会遍这个Class的所有方法,找出打了`@OnLifecycleEvent`的方法, 将这个`Method`和`callType`(callType是是否有参数的常量),封装成`MethodReference`对象, 将这些有注解的方法列表添加进一个集合,包装成``CallbackInfo`对象,以后通过`CallbackInfo`就可以调用到这些方法,

实现如下:

```java
boolean hasLifecycleMethods = ClassesInfoCache.sInstance.hasLifecycleMethods(klass);

nLifecycleEvent annotation = method.getAnnotation(OnLifecycleEvent.class);

createInfo(klass, methods);
    OnLifecycleEvent annotation = method.getAnnotation(OnLifecycleEvent.class);
	Lifecycle.Event event = annotation.value();
	int callType = CALL_TYPE_NO_ARG;
	MethodReference methodReference = new MethodReference(callType, method);
		handlers.put(newHandler, newEvent);
	CallbackInfo info = new CallbackInfo(handlerToEvent);

```

**状态间的对应关系**

```java
private static Event upEvent(State state) {
    switch (state) {
        case INITIALIZED:
        case DESTROYED:
            return ON_CREATE;
        case CREATED:
            return ON_START;
        case STARTED:
            return ON_RESUME;
        case RESUMED:
            throw new IllegalArgumentException();
    }
    throw new IllegalArgumentException("Unexpected state value " + state);
    }
```

```java
private static Event downEvent(State state) {
    switch (state) {
        case INITIALIZED:
            throw new IllegalArgumentException();
        case CREATED:
            return ON_DESTROY;
        case STARTED:
            return ON_STOP;
        case RESUMED:
            return ON_PAUSE;
        case DESTROYED:
            throw new IllegalArgumentException();
    }
    throw new IllegalArgumentException("Unexpected state value " + state);
}
```



---

### 二 LiveData

#### 2.1 使用

```java
// 声明成员
private final MutableLiveData<Boolean> flag = new MutableLiveData<>();

// 注册监听
flag.observe(this, new Observer<Boolean>() {
    @Override
    public void onChanged(Boolean aBoolean) {
        Log.d(TAG, "onChanged: ");
    }
});

// 更新
flag.postValue(true);
flag.setValue(true);
```

#### 2.2 实现

2.2.1 observe 方法

```java
// owner = MainActivity
LifecycleBoundObserver wrapper = new LifecycleBoundObserver(owner, observer);
LifecycleBoundObserver(@NonNull LifecycleOwner owner, Observer<? super T> observer) {
    super(observer);
    mOwner = owner;
}

owner.getLifecycle().addObserver(wrapper);
```

2.2.2 setValue()

```java
setValue(value); @MutableLiveData
	super.setValue(value); @LiveData
		mVersion++;
		mData = value;
		dispatchingValue(null);
			// 状态,避免重复
			considerNotify(iterator.next().getValue());
				// 版本判断
				observer.mLastVersion >= mVersion
                // 版本更新
                observer.mLastVersion = mVersion;
				// 这里就调用到MainActivity的onChanged
				observer.mObserver.onChanged((T) mData);
```

2.2.3 postValue()

```java
flag.postValue(true);@LiveData
	super.postValue(value);
		ArchTaskExecutor.getInstance().postToMainThread(mPostValueRunnable);
			1.ArchTaskExecutor.getInstance()
                return ArchTaskExecutor();	// 返回
					mDefaultTaskExecutor = new DefaultTaskExecutor();
					mDelegate = mDefaultTaskExecutor;
            2.postToMainThread(mPostValueRunnable);
				mMainHandler = new Handler(Looper.getMainLooper());
				// runnable = mPostValueRunnable
				mMainHandler.post(runnable);


private final Runnable mPostValueRunnable = new Runnable() {
    @Override
    public void run() {
        Object newValue;
        synchronized (mDataLock) {
            newValue = mPendingData;
            mPendingData = NOT_SET;
        }
        //noinspection unchecked
        setValue((T) newValue);
    }
};
```

