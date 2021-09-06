#### 说明

本文主要分析Room框架Flowable如何实现数据库增删改查如何自动更新数据的流程.

**设计模式**

* 装饰器
* 观察者

**基本使用**

```java
@Query("select * from Student")
Flowable<List<Student>> getAll();

Flowable<List<Student>> flowable = database.userDao().getAll();
Disposable subscribe = flowable.subscribe(new Consumer<List<Student>>() {
    @Override
    public void accept(List<Student> students) throws Exception {

    }
});
```

##### 第一部分: 初始化真正的订阅类

```java
Flowable<List<Student>> flowable = database.userDao().getAll();

getAll()
//返回
return RxRoom.createFlowable(__db, table, callable)
    
@RestrictTo(RestrictTo.Scope.LIBRARY_GROUP)
public static <T> Flowable<T> createFlowable(db,table,callable) {
    //不分析线程切换
    ArchTaskExecutor.getIOThreadExecutor();	--> 					 ArchTaskExecutor.getIOThreadExecutor();
			ArchTaskExecutor#executeOnDiskIO
				mDiskIO.executeOnDiskIO(runnable);
    
   //database.getQueryExecutor() --> ArchTaskExecutor 最终返回
  Scheduler scheduler = Schedulers.from(database.getQueryExecutor());
    
  final Maybe<T> maybe = Maybe.fromCallable(callable);	--> MaybeFromCallable(callable)
      
  return createFlowable(database, tableNames)
                .observeOn(scheduler)
                .flatMapMaybe(new Function<Object, MaybeSource<T>>() {
                    @Override
                    public MaybeSource<T> apply(Object o) throws Exception {
                        return maybe;
                    }
                });
    }
	//step1.createFlowable
		Flowable.create(FlowableOnSubscribe)	--> FlowableCreate(source,mode) //source == FlowableOnSubscribe  mode == LATEST
   //step2.flatMapMaybe
        Flowable.flatMapMaybe(func) --> FlowableFlatMapMaybe(source,mapper) //func 返回 maybe(MaybeFromCallable), source == FlowableCreate  mapper == func
            
            
//database.userDao().getAll()  最终返回了 FlowableFlatMapMaybe
    

```

****

**阶段一结论: database.userDao().getAll()  最终返回了 FlowableFlatMapMaybe**



**第二部分: 订阅关系**

```java
Disposable subscribe = flowable.subscribe(new Consumer<List<Student>>() {
    @Override
    public void accept(List<Student> students) throws Exception {

    }
});

1.flowable.subscribe(Consumer)
	
	LambdaSubscriber<T> ls = new LambdaSubscriber<T>(onNext, onError, onComplete, onSubscribe);
	subscribe(ls);
		subscribeActual(s);	//LambdaSubscriber;
	
		//因为 database.userDao().getAll()  最终返回了 FlowableFlatMapMaybe
		FlowableFlatMapMaybe# subscribeActual(ls) // ls  = LambdaSubscriber
           source.subscribe(new FlatMapMaybeSubscriber<T, R>(s, mapper);
              Flowable.subscribe(ls);
                            //s == LambdaSubscriber   mapper == func == MaybeFromCallable
           //上面的source == FlowableCreate
			FlowableCreate#subscribeActual(s) // s == FlatMapMaybeSubscriber(LambdaSubscriber)
           
              BaseEmitter<T> emitter = new LatestAsyncEmitter<T>(t);//t == FlatMapMaybeSubscriber(LambdaSubscriber)
			  source.subscribe(emitter); //scource == FlowableOnSubscribe
                            
2.source.subscribe(emitter); 
//会调用下面的方法 emitter == LatestAsyncEmitter(FlatMapMaybeSubscriber(LambdaSubscriber))
return Flowable.create(new FlowableOnSubscribe<Object>() {
            public void subscribe(final FlowableEmitter<Object> emitter) throws Exception 		{
                final Observer observer = new Observer(tableNames) {
                    public void onInvalidated(@NonNull Set<String> tables) {
                        if (!emitter.isCancelled()) {
                            //每当有数据更新后会调用这里的方法
                            emitter.onNext(RxRoom.NOTHING);
                        }

                    }
                };
                if (!emitter.isCancelled()) {
                    //这里注册一个监听器用于增删改查完成后通知
                    database.getInvalidationTracker().addObserver(observer);
                    emitter.setDisposable(Disposables.fromAction(new Action() {
                        public void run() throws Exception {
                            database.getInvalidationTracker().removeObserver(observer);
                        }
                    }));
                }

                if (!emitter.isCancelled()) {
                    emitter.onNext(RxRoom.NOTHING);
                }

            }
        }, BackpressureStrategy.LATEST);
             
```

****

**第三部分: emitter.onNext如何发送数据**

```java
//emitter == LatestAsyncEmitter(FlatMapMaybeSubscriber(LambdaSubscriber))

LatestAsyncEmitter#onNext
    drain();
		final Subscriber<? super T> a = actual;	//actual == FlatMapMaybeSubscriber
		a.onNext(o);

FlatMapMaybeSubscriber#onNext
    MaybeSource<? extends R> ms = mapper.apply(t); //ms == MaybeFromCallable(callable)
	InnerObserver inner = new InnerObserver();
	ms.subscribe(inner);
		Maybe.subscribe(obs)	//obs == InnerObserver
            MaybeFromCallable.subscribeActual(obs)	//obs == InnerObserver
            	T v = callable.call();
				observer.onSuccess(v);	//observer == InnerObserver
					InnerObserver#innerSuccess()
                        actual.onNext(value);	//actual == LambdaSubscriber
							LambdaSubscriber#onNext
                                onNext.accept(t);	// onNext == Consummer.accept 数据调用结束

```

****

**第四部分: RxRoom增删改查如何通知更新**

```java
//注册观察者 4.1
Flowable.create(new FlowableOnSubscribe<Object>() {
    @Override
    public void subscribe(final FlowableEmitter<Object> emitter){
        InvalidationTracker.Observer observer = new InvalidationTracker.Observer({
            onInvalidated( Set<String> tables) {
                emitter.onNext(NOTHING);
            }
        });
        database.getInvalidationTracker().addObserver(observer);
    }
}  
```

```
//StudentDao_Impl
public void update(Student student) {
    __db.beginTransaction();
    __db.endTransaction();
}
```

`__db.endTransaction();`

```java
RoomDatabase#endTransaction	//结束事务

	mInvalidationTracker.refreshVersionsAsync();	//刷新

	mDatabase.getQueryExecutor().execute(mRefreshRunnable);	//调用ArchTaskExecutor.execute

	Runnable mRefreshRunnable = new Runnable(){
        run() {
            //调用观察者检查更新
            synchronized (mObserverMap) {
                    for (Map.Entry<Observer, ObserverWrapper> entry : mObserverMap) {
                        entry.getValue().checkForInvalidation(mTableVersions);
                    }
                }
        }
    }
	
	InvalidationTracker#checkForInvalidation
        mObserver.onInvalidated(invalidatedTables);	//执行重新查询数据库 最终执行到4.1处emitter.onNext(NOTHING);
	

```

**结束**

没有线程切换的逻辑,

核心就是所有的增删改查操作都会调用roomdatase里面的`endTransaction`方法

`endTransaction`里面会去通知观察者进行更新