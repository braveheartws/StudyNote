```java
CallEnqueueObservable.java;
CallExecuteObservable.java;

RxJava2CallAdapter implements CallAdapter{
	RxJava2CallAdapterFactory create() --> RxJava2CallAdapter    
}

```

### 二. 代码分析

#### 2.1 基础流程分析

```java
public Retrofit build() {

    okhttp3.Call.Factory callFactory = this.callFactory;
    if (callFactory == null) {
        callFactory = new OkHttpClient();
    }
	// PlatForm$Android$MainThreadExecutor
    Executor callbackExecutor = this.callbackExecutor;
    if (callbackExecutor == null) {
        callbackExecutor = platform.defaultCallbackExecutor();
    }
	
    List<CallAdapter.Factory> callAdapterFactories = new ArrayList<>(this.callAdapterFactories);
    // 这里有RxJava2CallAdapterFactory，CompletableFutureCallAapterFactory
    // DefaultCallAdapterFactory
    callAdapterFactories.addAll(platform.defaultCallAdapterFactories(callbackExecutor));
	
    // Make a defensive copy of the converters.
    List<Converter.Factory> converterFactories =
        new ArrayList<>(
        1 + this.converterFactories.size() + platform.defaultConverterFactoriesSize());

    // Add the built-in converter factory first. This prevents overriding its behavior but also
    // ensures correct behavior when using converters that consume all types.
    converterFactories.add(new BuiltInConverters());
    converterFactories.addAll(this.converterFactories);
    converterFactories.addAll(platform.defaultConverterFactories());

    return new Retrofit(
        callFactory,
        baseUrl,
        unmodifiableList(converterFactories),
        unmodifiableList(callAdapterFactories),
        callbackExecutor,
        validateEagerly);
}
```

```java
// 动态代理里面的方法转换
Proxy.newProxyInstance...;
InvocationHandler#invoke() {
	return loadServiceMethod(method).invoke(args);
}

ServiceMethod<?> loadServiceMethod(Method method) {
    // 缓存判断
    ServiceMethod<?> result = serviceMethodCache.get(method);
    if (result != null) return result;

    synchronized (serviceMethodCache) {
      result = serviceMethodCache.get(method);
      if (result == null) {
        result = ServiceMethod.parseAnnotations(this, method);
        // 将CallAdapted 添加进缓存
        serviceMethodCache.put(method, result);
      }
    }
    return result;
  }

RequestFactory requestFactory = RequestFactory.parseAnnotations(retrofit, method);
HttpServiceMethod.parseAnnotations(retrofit, method, requestFactory);

static <ResponseT, ReturnT> HttpServiceMethod<ResponseT, ReturnT> parseAnnotations(
      Retrofit retrofit, Method method, RequestFactory requestFactory) {
	CallAdapter<ResponseT, ReturnT> callAdapter =
        createCallAdapter(retrofit, method, adapterType, annotations);
    Converter<ResponseBody, ResponseT> responseConverter =
        createResponseConverter(retrofit, method, responseType);
    // requestFactory = RequestFactory 
    // callFactory = OkHttpClient
    // responseConverter = GsonResponseBodyConverter
    // callAdapter = RxJava2CallAdapter
    return new CallAdapted<>(requestFactory, callFactory, responseConverter, callAdapter);
}

```

这里的`dapterType`就是返回类型，如果接口里面是如下这种方式

```java
@GET("banner/json")
Observable<BannerBean> getBanner();
```

那么返回类型就为`class io.reactivex.Observable`，最终`callAdapter`就会返回`RxJava2CallAdapter`。

`responseConverter`同上面的判断方式一样，因为都有默认的两个`CallAdapter`和`ConvertFactory`，我们一般都会添加`GsonConverterFactory`，调用`responseBodyConverter`方法时，最终返回`GsonResponseBodyConverter`,当发生转换时会调用如下代码

```java
public T convert(ResponseBody value) throws IOException {
    JsonReader jsonReader = gson.newJsonReader(value.charStream());
    try {
      T result = adapter.read(jsonReader);
      if (jsonReader.peek() != JsonToken.END_DOCUMENT) {
        throw new JsonIOException("JSON document was not fully consumed.");
      }
      return result;
    } finally {
      value.close();
    }
}
```



**在何处调用的RxJava2CallAdapter的adapt方法？**

因为在动态代理里面有一个钩子函数`loadServiceMethod`，上面分析过最终会将`CallAdapted`添加进`serviceMethodCache`缓存，调用`loadServiceMethod`的`invoke`方法的调用流程是怎样的? 因为`ServiceMethod`的子类是`HttpServiceMethod`,`HttpServiceMethod`的子类是`CallAdapted`，`ServiceMethod`里面有一个抽象方法`invoke`，`abstract @Nullable T invoke(Object[] args);`,所以`loadServiceMethod`的`invoke`的方法会调用到`HttpServiceMethod`里面的`invoke`方法：

```java
@Override
final @Nullable ReturnT invoke(Object[] args) {
    Call<ResponseT> call = new OkHttpCall<>(requestFactory, args, callFactory, responseConverter);
    return adapt(call, args);
}

protected abstract @Nullable ReturnT adapt(Call<ResponseT> call, Object[] args);

// CallAdapted
@Override
protected ReturnT adapt(Call<ResponseT> call, Object[] args) {
    return callAdapter.adapt(call);
}
```

```java
public Object adapt(Call<R> call) {
    Observable<Response<R>> responseObservable =
        isAsync ? new CallEnqueueObservable<>(call) : new CallExecuteObservable<>(call);

    Observable<?> observable;	// CallExecuteObservable
    observable = responseObservable;
    ...
    return RxJavaPlugins.onAssembly(observable);
}
```

##### 2.1.1 普通的Call流程分析

```java
ApiService apiService = RequestManager.sInstance().create(ApiService.class);
apiService.getBanner().enqueue(new Callback<BannerBean>() {
    @Override
    public void onResponse(Call<BannerBean> call, Response<BannerBean> response) {

    }

    @Override
    public void onFailure(Call<BannerBean> call, Throwable t) {

    }
});
```

```java
// OkHttpCall.java
public void enqueue(final Callback<T> callback) {
    
    okhttp3.Call call;
    Throwable failure;

    synchronized (this) {
      // 防止重复提交
      executed = true;

      call = rawCall;
      failure = creationFailure;
      if (call == null && failure == null) {
        try {
          // 返回RealCall
          call = rawCall = createRawCall();
        } catch (Throwable t) {
          throwIfFatal(t);
          failure = creationFailure = t;
        }
      }
    }

    if (failure != null) {
      callback.onFailure(this, failure);
      return;
    }

    if (canceled) {
      call.cancel();
    }
	// ReadCall.enqueue()
    call.enqueue(
        new okhttp3.Callback() {
          @Override
          public void onResponse(okhttp3.Call call, okhttp3.Response rawResponse) {
            Response<T> response;
            try {
              response = parseResponse(rawResponse);
            } catch (Throwable e) {
              throwIfFatal(e);
              callFailure(e);
              return;
            }

            try {
              callback.onResponse(OkHttpCall.this, response);
            } catch (Throwable t) {
              throwIfFatal(t);
              t.printStackTrace(); // TODO this is not great
            }
          }

          @Override
          public void onFailure(okhttp3.Call call, IOException e) {
            callFailure(e);
          }

          private void callFailure(Throwable e) {
            try {
              callback.onFailure(OkHttpCall.this, e);
            } catch (Throwable t) {
              throwIfFatal(t);
              t.printStackTrace(); // TODO this is not great
            }
          }
        });
  }
```

##### 2.1.2 创建RealCall

```java
private okhttp3.Call createRawCall() throws IOException {
    // 返回RealCall
    okhttp3.Call call = callFactory.newCall(requestFactory.create(args));
    ...
    return call;
}

```

##### 2.1.3 返回RealCall

```kotlin
// OkHttpClient
override fun newCall(request: Request): Call = RealCall(this, request, forWebSocket = false)
```

##### 2.1.4 将AsyncCall添加进队列

```java
// ReakCall
override fun enqueue(responseCallback: Callback) {
    check(executed.compareAndSet(false, true)) { "Already Executed" }

    callStart()
    client.dispatcher.enqueue(AsyncCall(responseCallback))
}
```

##### 2.1.5  将请求添加进准备的队列

```kotlin
internal fun enqueue(call: AsyncCall) {
    synchronized(this) {
      // 添加进准备好的队列
      readyAsyncCalls.add(call)
    }
    promoteAndExecute()
}
```

##### 2.1.6 将请求从准备好的队列移除添加到正在运行的队列

```kotlin
private fun promoteAndExecute(): Boolean {
    this.assertThreadDoesntHoldLock()
	// 创建可执行任务列表
    val executableCalls = mutableListOf<AsyncCall>()
    val isRunning: Boolean
    synchronized(this) {
      val i = readyAsyncCalls.iterator()
      while (i.hasNext()) {
        val asyncCall = i.next()
		// 如果已经超出最大请求数64,则这个任务不会被添加进执行队列
        if (runningAsyncCalls.size >= this.maxRequests) break // Max capacity.
        if (asyncCall.callsPerHost.get() >= this.maxRequestsPerHost) continue // Host max capacity.

        i.remove()
        asyncCall.callsPerHost.incrementAndGet()
        // 添加进执行队列
        executableCalls.add(asyncCall)
        // 添加进正在运行的队列
        runningAsyncCalls.add(asyncCall)
      }
      isRunning = runningCallsCount() > 0
    }

    for (i in 0 until executableCalls.size) {
      val asyncCall = executableCalls[i]
      // 2.1.7
      asyncCall.executeOn(executorService)
    }

    return isRunning
  }
```

##### 2.1.7 开始执行RealCall

```kotlin
fun executeOn(executorService: ExecutorService) {
    client.dispatcher.assertThreadDoesntHoldLock()

    var success = false
    try {
        // 线程池执行RealCall
        executorService.execute(this)
        success = true
    } catch (e: RejectedExecutionException) {
        val ioException = InterruptedIOException("executor rejected")
        ioException.initCause(e)
        noMoreExchanges(ioException)
        responseCallback.onFailure(this@RealCall, ioException)
    } finally {
        if (!success) {
            client.dispatcher.finished(this) // This call is no longer running!
        }
    }
}
```

##### 2.1.8 RealCall run方法

```kotlin
// 线程池最终执行run方法
override fun run() {
      threadName("OkHttp ${redactedUrl()}") {
        var signalledCallback = false
        timeout.enter()
        try {
          val response = getResponseWithInterceptorChain()
          signalledCallback = true
          responseCallback.onResponse(this@RealCall, response)
        } catch (e: IOException) {
          if (signalledCallback) {
            // Do not signal the callback twice!
            Platform.get().log("Callback failure for ${toLoggableString()}", Platform.INFO, e)
          } else {
            responseCallback.onFailure(this@RealCall, e)
          }
        } catch (t: Throwable) {
          cancel()
          if (!signalledCallback) {
            val canceledException = IOException("canceled due to $t")
            canceledException.addSuppressed(t)
            responseCallback.onFailure(this@RealCall, canceledException)
          }
          throw t
        } finally {
          client.dispatcher.finished(this)
        }
      }
    }
```

##### 2.1.9 添加拦截器,责任链模式

```kotlin
@Throws(IOException::class)
  internal fun getResponseWithInterceptorChain(): Response {
    // Build a full stack of interceptors.
    val interceptors = mutableListOf<Interceptor>()
    interceptors += client.interceptors
    interceptors += RetryAndFollowUpInterceptor(client)
    interceptors += BridgeInterceptor(client.cookieJar)
    interceptors += CacheInterceptor(client.cache)
    interceptors += ConnectInterceptor
    if (!forWebSocket) {
      interceptors += client.networkInterceptors
    }
    interceptors += CallServerInterceptor(forWebSocket)

    val chain = RealInterceptorChain(
        call = this,
        interceptors = interceptors,
        index = 0,
        exchange = null,
        request = originalRequest,
        connectTimeoutMillis = client.connectTimeoutMillis,
        readTimeoutMillis = client.readTimeoutMillis,
        writeTimeoutMillis = client.writeTimeoutMillis
    )

    var calledNoMoreExchanges = false
    try {
      val response = chain.proceed(originalRequest)
      if (isCanceled()) {
        response.closeQuietly()
        throw IOException("Canceled")
      }
      return response
    } catch (e: IOException) {
      calledNoMoreExchanges = true
      throw noMoreExchanges(e) as Throwable
    } finally {
      if (!calledNoMoreExchanges) {
        noMoreExchanges(null)
      }
    }
  }


val response = interceptor.intercept(next)
```

#### 2.2 RxJava返回流程分析

接2.1分析，`CallAdapted`最终会把请求转换成`CallExecuteObservable`

分析`CallExecuteObservable`#`subscribe`的流程

```java
//subscribeOn(Schedulers.io()).observeOn(AndroidSchedulers.mainThread())
apiService.getBanner()
    .subscribe(new Consumer<BannerBean>() {
    @Override
    public void accept(BannerBean bannerBean) throws Exception {

    }
    }, new Consumer<Throwable>() {
    @Override
    public void accept(Throwable throwable) throws Exception {

    }
});
```

不分析线程切换的实现，

```java
CallExecuteObservable.subscribe

apiService.getBanner()
    .subscribe(new Consumer<BannerBean>()
return subscribe(onNext, onError, Functions.EMPTY_ACTION, Functions.emptyConsumer());
               
@SchedulerSupport(SchedulerSupport.NONE)
public final Disposable subscribe(Consumer<? super T> onNext, Consumer<? super Throwable> onError, Action onComplete, Consumer<? super Disposable> onSubscribe) {
        LambdaObserver<T> ls = new LambdaObserver<T>(onNext, onError, onComplete, onSubscribe);
    subscribe(ls);
    return ls;
}
```

`subscribe(ls);` 会调用到`subscribeActual(observer);`因为`Observable<?>`会被转换成`CallExecuteObservable`,所以直接看`CallExecuteObservable`的`subscribeActual`方法

```java
@Override
protected void subscribeActual(Observer<? super Response<T>> observer) {
    // Since Call is a one-shot type, clone it for each new observer.
    Call<T> call = originalCall.clone();
    CallDisposable disposable = new CallDisposable(call);
    try{
        Response<T> response = call.execute();
    	observer.onNext(response);
    } catch (Throwable t) {
        observer.onError(t);
    }
}
```

```java
final @Nullable ReturnT invoke(Object[] args) {
    Call<ResponseT> call = new OkHttpCall<>(requestFactory, args, callFactory, responseConverter);
    return adapt(call, args);
}
```

通过上面的代码可以看到`call`实际是`OkHttpCall`,所以接下来看`OkHttpCall#execute()`

```java
public Response<T> execute() throws IOException {
    okhttp3.Call call;
    synchronized (this) {
      call = getRawCall(); // RealCall
    }

    return parseResponse(call.execute());
}
```

```kotlin
// RealCall
override fun execute(): Response {
    try {
       // 将任务添加到正在运行的同步队列runningSyncCalls
      client.dispatcher.executed(this)
      return getResponseWithInterceptorChain()
    } finally {
      client.dispatcher.finished(this)
    }
}
```

#### 2.3 责任链

> getResponseWithInterceptorChain()

```kotlin
internal fun getResponseWithInterceptorChain(): Response {
    // Build a full stack of interceptors.
    val interceptors = mutableListOf<Interceptor>()
    interceptors += client.interceptors
    interceptors += RetryAndFollowUpInterceptor(client)
    interceptors += BridgeInterceptor(client.cookieJar)
    interceptors += CacheInterceptor(client.cache)
    interceptors += ConnectInterceptor
    if (!forWebSocket) {
      interceptors += client.networkInterceptors
    }
    interceptors += CallServerInterceptor(forWebSocket)

    val chain = RealInterceptorChain(
        call = this,
        interceptors = interceptors,
        index = 0,
        exchange = null,
        request = originalRequest,
        connectTimeoutMillis = client.connectTimeoutMillis,
        readTimeoutMillis = client.readTimeoutMillis,
        writeTimeoutMillis = client.writeTimeoutMillis
    )
    val response = chain.proceed(originalRequest)
    if (isCanceled()) {
        response.closeQuietly()
        throw IOException("Canceled")
    }
    return response
    ...
  }
```

**RetryAndFollowUpInterceptor**:

**BridgeInterceptor:** 对用户请求进行处理，

**CacheInterceptor**:

**ConnectInterceptor:** 1.确定请求类型，是HTTP1 还是 HTTP2；2.查找可用连接，如果有就返回，没有就创建新的连接

**CallServerInterceptor**：1.写入请求头

#### 2.4 连接池

1. 最大空闲连接数5
2. 最长存活时间5分钟