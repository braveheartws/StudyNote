### 宏定义

```c
一种最简单的宏的形式如下：
#define   宏名   替换文本
```

```c
#define M 5                // 宏定义
#define PI 3.14            //宏定义
int a[M];                  // 会被替换为： int a[5];
int b = M;                 // 会被替换为： int b = 5;
printf("PI = %.2f\n", PI); // 输出结果为： PI = 3.14
```

