## EventBus源码笔记

### 一. 基本使用

#### 1.1 注册/取消注册

```java
EventBus.getDefault().register(this);
EventBus.getDefault().unregister(this)
```

#### 1.2 接收事件

```
@Subscribe(threadMode = ThreadMode.MAIN, sticky = true)
public void receiveEvent(InnerEvent event) {
    Log.d(TAG, "receiveEvent: " + event.msg);
}
```

#### 1.3 依赖

```groovy
 api 'org.greenrobot:eventbus:3.2.0'
```

---

### 二. 流程分析

#### 2.1 初始化

```java
EventBus.getDefault();

public EventBus() {
    this(DEFAULT_BUILDER);
}

EventBus(EventBusBuilder builder) {
        logger = builder.getLogger();
        subscriptionsByEventType = new HashMap<>();
        typesBySubscriber = new HashMap<>();
        stickyEvents = new ConcurrentHashMap<>();
        mainThreadSupport = builder.getMainThreadSupport();
        mainThreadPoster = mainThreadSupport != null ? mainThreadSupport.createPoster(this) : null;
        backgroundPoster = new BackgroundPoster(this);
        asyncPoster = new AsyncPoster(this);
        indexCount = builder.subscriberInfoIndexes != null ? builder.subscriberInfoIndexes.size() : 0;
        subscriberMethodFinder = new SubscriberMethodFinder(builder.subscriberInfoIndexes,
                builder.strictMethodVerification, builder.ignoreGeneratedIndex);
        logSubscriberExceptions = builder.logSubscriberExceptions;
        logNoSubscriberMessages = builder.logNoSubscriberMessages;
        sendSubscriberExceptionEvent = builder.sendSubscriberExceptionEvent;
        sendNoSubscriberEvent = builder.sendNoSubscriberEvent;
        throwSubscriberException = builder.throwSubscriberException;
        eventInheritance = builder.eventInheritance;
        executorService = builder.executorService;
}
```

#### 2.2 订阅

```java
register(this);

public void register(Object subscriber) {
        Class<?> subscriberClass = subscriber.getClass();	// 找到对应的class
        List<SubscriberMethod> subscriberMethods = subscriberMethodFinder.findSubscriberMethods(subscriberClass);	// 获取类上订阅的方法
    		// Map<Class<?>, List<SubscriberMethod>> METHOD_CACHE = new ConcurrentHashMap<>();
    		// class 里面对应的有注解标记的方法缓存
    		List<SubscriberMethod> subscriberMethods = METHOD_CACHE.get(subscriberClass);
    		subscriberMethods = findUsingInfo(subscriberClass);
    			findUsingReflectionInSingleClass(findState);	// 反射获取注解信息
    				Subscribe subscribeAnnotation = method.getAnnotation(Subscribe.class);	// 反射得到的订阅信息
    				ThreadMode threadMode = subscribeAnnotation.threadMode();	// 订阅线程
    				// 将有@Subscribe标记的方法包装成SubscriberMethod对象添加进订阅的方法列表
    				findState.subscriberMethods.add(new SubscriberMethod(method, eventType, threadMode,
                                    subscribeAnnotation.priority(), subscribeAnnotation.sticky()));
    			return getMethodsAndRelease(findState);
    				// 返回所有有@Subscribe标记的方法
    				List<SubscriberMethod> subscriberMethods = new ArrayList<>(findState.subscriberMethods);
    		// 添加进缓存
    		METHOD_CACHE.put(subscriberClass, subscriberMethods);
    		// 返回
    		return subscriberMethods;
        synchronized (this) {
            for (SubscriberMethod subscriberMethod : subscriberMethods) {
                subscribe(subscriber, subscriberMethod);
            }
        }
    }
```

```java
// 事件及优先级
@Subscribe(threadMode = ThreadMode.MAIN, sticky = true, priority = 2)
public void receiveEvent0(InnerEvent event) {
    Log.d(TAG, "receiveEvent0: " + event.msg);
}

@Subscribe(threadMode = ThreadMode.MAIN, sticky = true, priority = 3)
public void receiveEvent1(InnerEvent event) {
    Log.d(TAG, "receiveEvent1: " + event.msg);
}

@Subscribe(threadMode = ThreadMode.MAIN, sticky = true, priority = 4)
public void receiveEvent2(InnerEvent event) {
    Log.d(TAG, "receiveEvent2: " + event.msg);
}

// 优先级为:receiveEvent2 receiveEvent1 receiveEvent0
```

```java
private void subscribe(Object subscriber, SubscriberMethod subscriberMethod) {
        Class<?> eventType = subscriberMethod.eventType;
    	// 将订阅者和方法封装成一个Subscription对象
        Subscription newSubscription = new Subscription(subscriber, subscriberMethod);
    	// Map<Class<?>, CopyOnWriteArrayList<Subscription>> subscriptionsByEventType;
    	// subscriptions: key:事件类型 v: Subscription对象
        CopyOnWriteArrayList<Subscription> subscriptions = subscriptionsByEventType.get(eventType);
        if (subscriptions == null) {
            // 缓存
            subscriptions = new CopyOnWriteArrayList<>();
            subscriptionsByEventType.put(eventType, subscriptions);
        }
		// 事件优先级排序
        int size = subscriptions.size();
        for (int i = 0; i <= size; i++) {
            if (i == size || subscriberMethod.priority > subscriptions.get(i).subscriberMethod.priority) {
                subscriptions.add(i, newSubscription);
                break;
            }
        }
		// typesBySubscriber: k: 订阅者 v: 事件类型
        List<Class<?>> subscribedEvents = typesBySubscriber.get(subscriber);
        if (subscribedEvents == null) {
            // 缓存
            subscribedEvents = new ArrayList<>();
            typesBySubscriber.put(subscriber, subscribedEvents);
        }
        subscribedEvents.add(eventType);
		
        if (subscriberMethod.sticky) {	// 如果是粘性事件
            if (eventInheritance) {	// 默认true
                Set<Map.Entry<Class<?>, Object>> entries = stickyEvents.entrySet();
                for (Map.Entry<Class<?>, Object> entry : entries) {
                    Class<?> candidateEventType = entry.getKey();
                    if (eventType.isAssignableFrom(candidateEventType)) {
                        Object stickyEvent = entry.getValue();
                        // 如果当前这个方法是接收粘性事件,那么检查粘性事件的集合
                        checkPostStickyEventToSubscription(newSubscription, stickyEvent);
                    }
                }
            } else {
                Object stickyEvent = stickyEvents.get(eventType);
                checkPostStickyEventToSubscription(newSubscription, stickyEvent);
            }
        }
}
```

```
// 将已保存的粘性事件发送
private void checkPostStickyEventToSubscription(Subscription newSubscription, Object stickyEvent) {
    if (stickyEvent != null) {
    	postToSubscription(newSubscription, stickyEvent, isMainThread());
    }
}
```

#### 2.3 发送事件

```java
public void post(Object event) {
		// 获取线程绑定对象
        PostingThreadState postingState = currentPostingThreadState.get();
        List<Object> eventQueue = postingState.eventQueue;
        eventQueue.add(event);

        if (!postingState.isPosting) {
            postingState.isMainThread = isMainThread();
            postingState.isPosting = true;
            if (postingState.canceled) {
                throw new EventBusException("Internal error. Abort state was not reset");
            }
            try {
                // 将事件队列的事件发送出去
                while (!eventQueue.isEmpty()) {
                    postSingleEvent(eventQueue.remove(0), postingState);
                }
            } finally {
                postingState.isPosting = false;
                postingState.isMainThread = false;
            }
        }
}
```

```java
private void postSingleEvent(Object event, PostingThreadState postingState) throws Error {
        Class<?> eventClass = event.getClass();
        boolean subscriptionFound = false;
        if (eventInheritance) {
            // lookupAllEventTypes 返回这个事件Class的父Class及接口Class
            List<Class<?>> eventTypes = lookupAllEventTypes(eventClass);
            int countTypes = eventTypes.size();
            for (int h = 0; h < countTypes; h++) {
                Class<?> clazz = eventTypes.get(h);
                subscriptionFound |= postSingleEventForEventType(event, postingState, clazz);
            }
        } else {
            subscriptionFound = postSingleEventForEventType(event, postingState, eventClass);
        }
        if (!subscriptionFound) {
            if (logNoSubscriberMessages) {
                logger.log(Level.FINE, "No subscribers registered for event " + eventClass);
            }
            if (sendNoSubscriberEvent && eventClass != NoSubscriberEvent.class &&
                    eventClass != SubscriberExceptionEvent.class) {
                post(new NoSubscriberEvent(this, event));
            }
        }
}
```

```java
private boolean postSingleEventForEventType(Object event, PostingThreadState postingState, Class<?> eventClass) {
        CopyOnWriteArrayList<Subscription> subscriptions;
        synchronized (this) {
            // 获取到之前保存的事件类型和订阅信息
            subscriptions = subscriptionsByEventType.get(eventClass);
        }
        if (subscriptions != null && !subscriptions.isEmpty()) {
            for (Subscription subscription : subscriptions) {
                postingState.event = event;
                postingState.subscription = subscription;
                boolean aborted;
                try {
                    // 发送事件
                    postToSubscription(subscription, event, postingState.isMainThread);
                    aborted = postingState.canceled;
                } finally {
                    postingState.event = null;
                    postingState.subscription = null;
                    postingState.canceled = false;
                }
                if (aborted) {
                    break;
                }
            }
            return true;
        }
        return false;
}
```

```java
private void postToSubscription(Subscription subscription, Object event, boolean isMainThread) {
        switch (subscription.subscriberMethod.threadMode) {
            case POSTING:
                invokeSubscriber(subscription, event);
                break;
            case MAIN:
                if (isMainThread) {
                    // 2.3.1
                    invokeSubscriber(subscription, event);
                } else {
                    // 2.3.2
                    mainThreadPoster.enqueue(subscription, event);
                }
                break;
            case MAIN_ORDERED:
                if (mainThreadPoster != null) {
                    mainThreadPoster.enqueue(subscription, event);
                } else {
                    // temporary: technically not correct as poster not decoupled from subscriber
                    invokeSubscriber(subscription, event);
                }
                break;
            case BACKGROUND:
                if (isMainThread) {
                    backgroundPoster.enqueue(subscription, event);
                } else {
                    invokeSubscriber(subscription, event);
                }
                break;
            case ASYNC:
                asyncPoster.enqueue(subscription, event);
                break;
            default:
                throw new IllegalStateException("Unknown thread mode: " + subscription.subscriberMethod.threadMode);
        }
    }
```

##### 2.3.1 同一个线程

```java
void invokeSubscriber(Subscription subscription, Object event) {
        try {
            // 反射的method调用方式
            subscription.subscriberMethod.method.invoke(subscription.subscriber, event);
        } catch (InvocationTargetException e) {
            handleSubscriberException(subscription, event, e.getCause());
        } catch (IllegalAccessException e) {
            throw new IllegalStateException("Unexpected exception", e);
        }
}
```

##### 2.3.2 发送其他线程执行

```java
public void enqueue(Subscription subscription, Object event) {
    	// 将事件对象,订阅信息封装成PendingPost
        PendingPost pendingPost = PendingPost.obtainPendingPost(subscription, event);
        synchronized (this) {
            queue.enqueue(pendingPost);
            if (!handlerActive) {
                handlerActive = true;
                if (!sendMessage(obtainMessage())) {
                    throw new EventBusException("Could not send handler message");
                }
            }
        }
}
```

```java
@Override
public void handleMessage(Message msg) {
        boolean rescheduled = false;
        try {
            long started = SystemClock.uptimeMillis();
            while (true) {
                PendingPost pendingPost = queue.poll();
                if (pendingPost == null) {
                    synchronized (this) {
                        // Check again, this time in synchronized
                        pendingPost = queue.poll();
                        if (pendingPost == null) {
                            handlerActive = false;
                            return;
                        }
                    }
                }
                eventBus.invokeSubscriber(pendingPost);
                long timeInMethod = SystemClock.uptimeMillis() - started;
                if (timeInMethod >= maxMillisInsideHandleMessage) {
                    if (!sendMessage(obtainMessage())) {
                        throw new EventBusException("Could not send handler message");
                    }
                    rescheduled = true;
                    return;
                }
            }
        } finally {
            handlerActive = rescheduled;
        }
}
```

```java
void invokeSubscriber(PendingPost pendingPost) {
        Object event = pendingPost.event;
        Subscription subscription = pendingPost.subscription;
        PendingPost.releasePendingPost(pendingPost);
        if (subscription.active) {
            // 2.3.1
            invokeSubscriber(subscription, event);
        }
}
```

#### 2.4 粘性事件

```java
public void postSticky(Object event) {
        synchronized (stickyEvents) {
            // 将事件添加到粘性列表
            stickyEvents.put(event.getClass(), event);
        }
        // Should be posted after it is putted, in case the subscriber wants to remove immediately
        post(event);
}
```

---

### 三. 总结

> 在订阅的时候通过反射获取对应方法注解, 将订阅者对象,和订阅的方法封装在一个Subscription对象里面, 同时会将事件类型作为key,事件对应的所有订阅过的列表保存在`subscriptionsByEventType`里面,当发送事件时根据,事件类型去`subscriptionsByEventType`获取`CopyOnWriteArrayList<Subscription>`列表,然后遍历这个列表里面的`Subscription`对象,根据`threadMode`判断属于哪个线程处理,如果是不需要做线程切换则直接将最开始的反射保存的`Methd`对象,通过反射调用. 如果是异步线程则需要将事件发送到所在的队列,由对应的线程从队列里面取出消息,再通过封装的`PendingPost`对象,通过`Subscription`里面的`Method`反射调用来传输数据
>
> 1. 粘性事件会一直存在内存,需要移除
> 2. 注册后需要取消注册,避免内存泄漏



