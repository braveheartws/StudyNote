## Glide源码

### 一. 基本使用

#### 1.1依赖

```groovy
api 'org.greenrobot:eventbus:3.2.0'
```

#### 1.2 普通加载图片

```java
String url = "http://guolin.tech/book.png";
Glide.with(this).load(url).into(binding.imageView);
```

#### 1.3 圆角图片

```java
Glide.with(this).load(url)
    .transform(new RoundedCorners(100))
    .into(binding.imageView2);

//控制四个角的大小
Glide.with(mContext).load("图片路径url")
	.apply(RequestOptions.bitmapTransform(new RoundedCorners(50)))//圆角半径
	.into(R.id.new_daiban_touxiang_item_img);//ImageView的id
```

#### 1.4 圆形图片

```java
RequestOptions mRequestOptions = RequestOptions.circleCropTransform()
                .diskCacheStrategy(DiskCacheStrategy.NONE)//不做磁盘缓存
                .skipMemoryCache(true);//不做内存缓存
Glide.with(this).load(url)
	.apply(mRequestOptions)
	.into(binding.imageView3);
```

---

### 二. 代码分析

#### 2.1 Glide.with(this)

```java
GeneratedAppGlideModule =  GeneratedAppGlideModuleImpl;

```

