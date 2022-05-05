## 第一节：指针与数组名

### 1.1 通过数组名访问数组元素

下面的代码可行

```c
int i, a[] = {3,4,5,6,7,3,7,4,4,6}; 
for (i = 0; i <= 9; i++) 
{ 
     printf("%d\n", a[i]); 
}
```

```c
int i, a[] = {3,4,5,6,7,3,7,4,4,6}; 
for (i = 0; i <= 9; i++) 
{ 
     printf("%d\n", *(a+i)); 
}
```

### 1.2 通过指针访问元素

下面的代码也是可行

```c
int i, *pa, a[] = {3,4,5,6,7,3,7,4,4,6}; 
pa = a; /*请注意数组名 a 直接赋值给指针 pa*/ 
for (i = 0; i <= 9; i++) 
{ 
     printf("%d\n", pa[i]); 
}
```

```c
int i, *pa, a[] = {3,4,5,6,7,3,7,4,4,6}; 
pa = a; 
for (i = 0; i <= 9; i++) 
{ 
 printf("%d\n", *(pa+i)); 
}
```

### 1.3 数组名与指针变量的区别

下面的代码可行

```c
int i, *pa, a[] = {3,4,5,6,7,3,7,4,4,6}; 
pa = a; 
for (i = 0; i <= 9; i++) 
{ 
    printf("%d\n", *pa); 
    pa++; /*注意这里，指针值被修改* 这个代码与上面的代码不同的是，指针 pa 在整个循环中，其值是不断递增的，		即指针值被修改了。/ 
}
```



下面的代码有问题

```c
for (i = 0; i <= 9; i++) {
    printf("%d\n",*a);
    a++;    // 数组名是指针常量，其值是不能修改的，因此不能类似这样操作：a++。
}
```

总结：

* pa指针是指针变量，而**数组名只是一个指针常量**

前面pa[i]，*(pa+i)处，指针 pa 的值是使终没有改变。所以变量指针 pa 与数组名 a 可以互换。

### 1.4 声明指针常量

测试代码：

```c
int i, a[] = {3,4,5,6,7,3,7,4,4,6}; 
int *const pa = a; /* 注意 const 的位置：不是 const int *pa */ 
for (i = 0; i <= 9; i++) 
{ 
     printf("%d\n", *pa); 
     pa++ ; /*注意这里，指针值被修改*/ 
}
```

---

## 第二节：**const int \*pi**与int \*const pi**的区别**

测试代码：

```c
const int i = 20;
// 等价于
int const i = 20;

const int *pi;
// 等价于
int const *pi
```

* **int 与const哪个放前哪个放后都是一样的**

### 2.1 const int \*pi的语义

示例代码：

```c
int i1 = 50, i2 = 60;
const int *pi = &i1;
pi = &i2;	// 可以
*pi = 80;	// 不可以

int * const pInt = &i1;
// pInt = &i2; 这也是错误的 因为const 修饰的p指向的位置,能修改这块内存里面的值，但是不能将p这个指向改变为其他的
```

为什么`*pi = 80;`不可以呢？

const修饰的是整个\*pi，所以*pi 是常量，是不能被赋值的。

pi 前并没有用 const 修饰，所以 pi 是指针变量，能被赋值重新指向另一内存地址的。



### 2.2 再看int *const pi

测试代码：

```c
int i1 = 30; 
int i2 = 40; 
int *const pi = &i1;

```

这个pi被const修饰，**它只能永远指向初始化时的内存地址**，总结：

* 1）pi 因为有了 const 的修饰，所以只是一个指针常量：也就是说 pi 值是不可修改的（即 pi 不可以重新指向 i2 这个变量了）（请看第 4 行的注释）。

* 2）整个*pi 的前面没有 const 的修饰。也就是说，*pi 是变量而不是常量，所以我们可以通过*pi 来修改它所指内存 i1 的值（请看第 5 行的注释）。

**这次的** **pi** **是一个指向** **int** **变量类型数据的指针常量**

* 1） 如果 const 修饰在*pi 前，则不能改的是*pi(即不能类似这样：*pi=50;赋值)而不是指 pi。

* 2） 如果 const 是直接写在 pi 前，则 pi 不能改(即不能类似这样：pi=&i;赋值)。

### 2.3 指针常量与常量指针

常量指针：

```c
const int *ptr;
```

这个指针指向的是一个常量，指针的指向可以修改，但不能修改指向的值；比如上面的代码 const修饰的是*ptr，那就说明不能通过`*ptr=50;`这种方式去修改值。但可以通过`ptr=&b`，去修改指向。

指针常量

```c
int * const ptr;
```

这个const修饰的是ptr，也就是说ptr不能再改变，但是可以通过`*ptr = 50`这种方式来修改指向内存的值。



这里可以发现规律：const 在 * 前面，读作常量指针； * 在 const 前面，读作指针常量。

```c
int a = 8,b = 9;
int *ptr = &a;	// 指针变量
int *const ptr = &b; // 指针常量
int const *ptr = &a; // 常量指针
```





## 第三节 函数参数的传递

C 语言中函数参数的传递有：值传递、地址传递、引用传递这三种形式

### 3.1 值传递

示例代码：

```c
void Exchg1(int x, int y) /* 定义中的x,y变量被称为Exchg1函数的形式参数 */
{
    int tmp;
    tmp = x;
    x = y;
    y = tmp;
    printf("x = %d, y = %d.\n", x, y);
}

int main(int argc, char *argv[]) {
    int a = 4,b = 6;
    Exchg1(a, b); /*a,b 变量为 Exchg1 函数的实际参数。*/
    printf("a = %d, b = %d.\n", a, b);
    return(0);
}

// x = 6, y = 4.
// a = 4, b = 6.
```

为什么会交换失败？

因为这里int x = a；相当于把a的值传递给了参数x，所以方法内部不会改变外部a的值

### 3.2 地址传递

示例代码：

```c
void Exchg2(int *px, int *py) 
{ 
    // px = &a; /* ← */ 
	// py = &b; /* ← 请注意这两行，它是调用 Exchg2 的隐含动作。*/
    int tmp = *px; 
    *px = *py; 
    *py = tmp; 
    printf("*px = %d, *py = %d.\n", *px, *py); 
}
main() 
{ 
    int a = 4; 
    int b = 6; 
    Exchg2(&a, &b); 
    printf("a = %d, b = %d.\n”, a, b); 
    return(0); 
}
// *px = 6, *py = 4. 
//  a = 6, b = 4.
```

Exchg2(int *px, int *py);有两个隐含操作：

```c
px = &a;
py = &b;
```

**有了头两行的隐含赋值操作。我们现在已经可以看出，指针** *px、\*py的值已经分别是a、b **变量的地址值了。接下来，对\*px、*****py的操作当然也就是对a、b **变量本身的操作了**



### 3.3 引用传递

示例代码：

```c
void Exchg3(int &x, int &y) /* 注意定义处的形式参数的格式与值传递不同 */
{
    int tmp = x;
    x = y;
    y = tmp;
    printf("x = %d, y = %d.\n", x, y);
}
```

x，y分别引用了a，b变量；

值传递与引用传递作一个比较：

* 在函数定义格式上有不同：

  ​	值传递在定义处是：Exchg1(int x, int y); 

  ​	引用传递在这义处是：Exchg3(int &x, int &y); 

* 调用时有相同的格式：

  ​	值传递：Exchg1(a, b); 

  ​	引用传递：Exchg3(a, b);

* 功能上是不同的：

  ​	值传递的函数里操作的不是 a、b 变量本身，只是将 a、b 值赋给了 x、y函数里操作的只是 x、y 变量而不	是 a、b，显示 a、b 的值不会被 Exchg1 函数所修改。

  ​	引用传递 Exchg3(a, b)函数里是用 a、b 分别代替了 x、y。函数里操作的就是 a、b 变量的本身，因此 a、b 	的值可在函数里被修改的。

## 第四节 二级指针

示例代码：

```java
void find2(char array[], char search, char **ppa) {
    ppa = &p; // 这是一个隐含操作
    *ppa = array + i
}

```

ppa 指向指针 p 的地址。

对*ppa 的修改就是对 p 值的修改。



## 第五节：函数名与函数指针

### 5.1 通常的函数调用

示例代码：

```java
void MyFun(int x); /* 此处的声明也可写成：void MyFun(int) */ 
int main(int argc, char* argv[]) 
{ 
    MyFun(10); /* 这里是调用 MyFun(10) 函数 */ 
    return(0); 
} 
void MyFun(int x) /* 这里定义一个 MyFun 函数 */ 
{ 
    printf("%d\n",x); 
}
```

### 5.2 函数指针

示例代码：

```c
void (*FunP)(int);
void (*FunP)(int x);
```

FunP作为指针变量可以指向任何具有其它相同参数以及返回值的函数。

### 5.3 通过函数指针调用函数；

示例代码：

```c
void MyFun(int x);
void (*FunP)(int);  /*也可声明成 void(*FunP)(int x)，但习惯上一般不这样。 */
int main(int argc, char* argv[]) 
{
    MyFun(10); /* 这是直接调用 MyFun 函数 */
    FunP = &MyFun; /* 将 MyFun 函数的地址赋给 FunP 变量 */
    (*FunP)(20); /* （★）这是通过函数指针变量 FunP 来调用MyFun 函数的。 */
    return 0;
}
void MyFun(int x) /* 这里定义一个 MyFun 函数 */ 
{ 
    printf("%d\n",x); 
}
```

### 5.4 调用函数的其他书写格式

示例代码：

```c
void MyFun(int x);
void (*FunP)(int);  /*也可声明成 void(*FunP)(int x)，但习惯上一般不这样。 */
int main(int argc, char* argv[]) 
{
    FunP = MyFun; /* 将 MyFun 函数的地址赋给 FunP 变量 */
    FunP(20); // 也是通过函数指针来调用的 可行
    (*MyFun)(10);	// 这种方式也可以
    return 0;
}
```

总结：

* 其实，MyFun 的函数名与 FunP 函数指针都是一样的，即都是函数指针。MyFun 函数名是一个函数指针常量，而 FunP是一个函数数指针变量，这是它们的关系。
* 函数名调用如果都得如(*MyFun)(10)这样，那书写与读起来都是不方便和不习惯的。所以 C 语言的设计者们才会设计成又可允许 MyFun(10)这种形式地调用
* 为统一起见，FunP 函数指针变量也可以 FunP(10)的形式来调用。
* 赋值时，即可 FunP = &MyFun 形式，也可 FunP = MyFun。

补充说明一点，在函数的声明处：

```c
void MyFun(int); /*不能写成 void (*MyFun)(int)。*/ 

void (*FunP)(int); /*不能写成 void FunP(int)。*/ 
```



### 5.5 定义某一函数指针类型

示例代码：

```c
void MyFun(int x); /*此处的声明也可写成：void MyFun( int )*/
typedef void (*FunType)(int); /*（★）这样只是定义一个函数指针类型*/
FunType FunP; /*然后用 FunType 类型来声明全局 FunP 变量*/
int main(int argc, char* argv[]) 
{
    FunType FunP; /*函数指针变量当然也是可以是局部的 ，那就请在这里声明了。 */
    FunP = &MyFun;
    return 0;
}
```

*有了 FunType 类型后，我们就可以同样地、很方便地用 FunType 类型来声明多个同类型的函数指针变量了。

### 5.6函数指针作为某个函数的参数

示例代码：

```c
void Exchg1(int *x, int *y) /* 定义中的x,y变量被称为Exchg1函数的形式参数 */
{
    int tmp = *x;
    *x = *y;
    *y = tmp;
    printf("x = %d, y = %d.\n", *x,* y);
}
void CallExchg(void(*FunP)(int *, int *), int *x, int *y) {
    FunP(x, y);
}

int main(int argc, char *argv[]) {
    int a = 5, b = 6;
    void (*CallP)(void(*)(int *, int *),int*,int*);
    CallP = CallExchg;
    CallP(Exchg1, &a, &b);
    printf("%d, %d\n", a, b);
    return (0);
}
```

---

## 第六节 C风格字符串(C-style string)

### 6.1 基本介绍

示例代码：

```c
char str1[4] = {'a','b','c','\0'};	// 是C风格字符串
char arr2[4] = {'a','b','c','d'};	// 不是C风格字符串

```

* 这两个都是char数组，但只有第二个是字符串。空字符对C-风格字符串至关重要。C++有很多处理字符串的函数，包括cout使用的那些函数，它们都逐个地处理字符串中的字符，直到发现空字符为止。

* 如果使用cout显示上面的dog数组（它不是字符串），cout将打印出数组中的8个字母，并接着将[内存](https://so.csdn.net/so/search?q=内存&spm=1001.2101.3001.7020)中随后的各个字节解释为要打印的字符，直到遇到空字符为止。

\*将数组初始化为字符串的方法要使用大量的单引号，且必须记住加上空字符，这种方法十分麻烦。有一种更好的将字符数组初始化为字符串的方法——只需使用一个用双引号阔气的字符串即可，这种字符串被称为**字符串常量（string constant）**或**字符串字面值（string literal）**

```c
char bird[11] = "bird";   	// 结尾自动包含\0 ,虽然申明长度11，但是实际长度只有4
char fish[] = "Bubbles";    // 让编译器自动计数
char *dog = "dog";			// 有效
char ch = 'C';				// 有效，是一个字符
char ch1 = "C";				// 错误：类型不匹配，试图将一个内存地址赋给shirt_size
```

### 6.2 字符数组

示例代码：

```c
char* name[3] = {"abc","def","ghi"};

printf(" *name = %s\n",*name); //  *name = abc
printf(" *(name + 1) = %s\n",*(name + 1));   // *(name + 1) = def
printf(" *(name + 2) =  %s\n",*(name + 2));   // *(name + 2) =  ghi
printf(" abc ptr =  %p\n",(name + 0));   // abc ptr =  000000000061FE00
printf(" def ptr =   %p\n",(name + 1));   // def ptr =   000000000061FE08
printf(" ghi ptr =   %p\n",(name + 2));   // ghi ptr =   000000000061FE10
printf("--------------------------------------------\n");
printf("&name[0][0] = %p\n", &name[0][0]);	// &name[0][0] = 00000000004050EC
printf("&name[1][0] = %p\n", &name[1][0]);	// &name[1][0] = 00000000004050F0
printf("&name[2][0] = %p\n", &name[2][0]);	// &name[2][0] = 00000000004050F4

printf("(*(name + 0) + 0) = %p\n", (*(name + 0) + 0));	// (*(name + 0) + 0) = 00000000004050EC
printf("(*(name + 1) + 0) = %p\n", (*(name + 1) + 0)); // (*(name + 1) + 0) = 00000000004050F0
printf("(*(name + 2) + 0) = %p\n", (*(name + 2) + 0)); // (*(name + 2) + 0) = 00000000004050F4


printf("name -->  %p\n",name);
printf("&name -->  %p\n",&name);
printf("name[0] --> %p\n",name[0]);
printf("*name -->  %p\n",*name);
printf("--------------------------------------------\n");
printf(" *(*(name + 0) + 0) =  %c\n",*(*(name + 0) + 0));   //  *(*(name + 0) + 0) =  a
printf(" *(*(name + 0) + 1) = %c\n",*(*(name + 0) + 1));   // *(*(name + 0) + 1) = b
printf(" *(*(name + 0) + 2) =  %c\n",*(*(name + 0) + 2));   // *(*(name + 0) + 2) =  c

*name = abc
*(name + 1) = def
*(name + 2) =  ghi
abc ptr =  000000000061FE00
def ptr =   000000000061FE08
ghi ptr =   000000000061FE10
--------------------------------------------
&name[0][0] = 00000000004050EC
&name[1][0] = 00000000004050F0
&name[2][0] = 00000000004050F4
(*(name + 0) + 0) = 00000000004050EC
(*(name + 1) + 0) = 00000000004050F0
(*(name + 2) + 0) = 00000000004050F4

// 重点
name -->  000000000061FE00
&name -->  000000000061FE00
name[0] --> 00000000004050EC
*name -->  00000000004050EC

--------------------------------------------
*(*(name + 0) + 0) =  a
*(*(name + 0) + 1) = b
*(*(name + 0) + 2) =  c
```

可以发现：

* name的地址并不是`name[0][0]`的地址
* 字符串数组 !=  二维字符数组
* name 只是一个字符串数组名有自己的地址

```c
通过多次实验能得出结论：
char *animals[3] = {"dog","cat","pig"};
这个animals很像char类型的二级指针。

在传参的时候直接是char**;
void Test(char** animals) {
    println("index0=%s\n", *(animals + 0)); // *(animals + 0) = *animals
}
```

