**管理位图内存**

Android 位图内存管理的演变过程：

- 在 Android Android 2.2（API 级别 8）及更低版本上，当发生垃圾回收时，应用的线程会停止。这会导致延迟，从而降低性能。Android 2.3 添加了并发垃圾回收功能，这意味着系统不再引用位图后，很快就会回收内存。
- 在 Android 2.3.3（API 级别 10）及更低版本上，位图的后备像素数据存储在本地内存中。它与存储在 Dalvik 堆中的位图本身是分开的。本地内存中的像素数据并不以可预测的方式释放，可能会导致应用短暂超出其内存限制并崩溃。从 Android 3.0（API 级别 11）到 Android 7.1（API 级别 25），像素数据会与关联的位图一起存储在 Dalvik 堆上。在 Android 8.0（API 级别 26）及更高版本中，位图像素数据存储在原生堆中。

ps: 在4.4以前位图内存复用只能是宽高相等并且inSampleSize为1才会复用, 4.4开始被复用的Bitmap的内存必须大于需要申请的Bitmap的内存

Bitmap内存模型

| 实体/版本  | 5.0-7.1                              | 8.0                                  |
| ---------- | ------------------------------------ | ------------------------------------ |
| Bitmap对象 | Java Heap                            | Java Heap                            |
| 像素数据   | Java Heap(解决Native Bitmap内存泄漏) | Native Heap(共享整个系统内存避免oom) |



```java
static boolean canUseForInBitmap(
            Bitmap candidate, BitmapFactory.Options targetOptions) {

    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT) {
        // From Android 4.4 (KitKat) onward we can re-use if the byte size of
        // the new bitmap is smaller than the reusable bitmap candidate
        // allocation byte count.
        int width = targetOptions.outWidth / targetOptions.inSampleSize;
        int height = targetOptions.outHeight / targetOptions.inSampleSize;
        int byteCount = width * height * getBytesPerPixel(candidate.getConfig());
        return byteCount <= candidate.getAllocationByteCount();
    }

    // On earlier versions, the dimensions must match exactly and the inSampleSize must be 1
    return candidate.getWidth() == targetOptions.outWidth
        && candidate.getHeight() == targetOptions.outHeight
        && targetOptions.inSampleSize == 1;
}
```

