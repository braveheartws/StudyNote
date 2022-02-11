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

##### 2.1.1

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

##### 2.1.2

```java
private okhttp3.Call createRawCall() throws IOException {
    // 返回RealCall
    okhttp3.Call call = callFactory.newCall(requestFactory.create(args));
    ...
    return call;
}

```

##### 2.1.3

```kotlin
// OkHttpClient
override fun newCall(request: Request): Call = RealCall(this, request, forWebSocket = false)
```

##### 2.1.4

```java
// ReakCall
override fun enqueue(responseCallback: Callback) {
    check(executed.compareAndSet(false, true)) { "Already Executed" }

    callStart()
    client.dispatcher.enqueue(AsyncCall(responseCallback))
}
```

##### 2.1.5

```kotlin
internal fun enqueue(call: AsyncCall) {
    synchronized(this) {
      // 添加进准备好的队列
      readyAsyncCalls.add(call)
    }
    promoteAndExecute()
}
```

##### 2.1.6

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

##### 2.1.7

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

##### 2.1.8

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

