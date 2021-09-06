```c++
 1.prntf函数
printf("%s\n", );
format 标签属性是 %[flags][width][.precision][length]specifier
格式字符意义
d以十进制形式输出带符号整数(正数不输出符号)
o以八进制形式输出无符号整数(不输出前缀0)
x,X以十六进制形式输出无符号整数(不输出前缀Ox)
u以十进制形式输出无符号整数
f以小数形式输出单、双精度实数
e,E以指数形式输出单、双精度实数
g,G以%f或%e中较短的输出宽度输出单、双精度实数
c输出单个字符
s输出字符串
p输出指针地址
lu32位无符号整数
llu64位无符号整数
例：
#include <stdio.h>
int main()
{
   char ch = 'A';
   char str[20] = "www.runoob.com";
   float flt = 10.234;
   int no = 150;
   double dbl = 20.123456;
   printf("字符为 %c \n", ch);//A
   printf("字符串为 %s \n" , str);//www.runoob.com 
   printf("浮点数为 %f \n", flt);//10.234000 
   printf("整数为 %d\n" , no);//150
   printf("双精度值为 %lf \n", dbl);//20.123456
   printf("八进制值为 %o \n", no);//226
   printf("十六进制值为 %x \n", no);//96
   return 0;
}

%d 十进制有符号整数
%u 十进制无符号整数
%f 浮点数
%s 字符串
%c 单个字符
%p 指针的值
%e 指数形式的浮点数
%x, %X 无符号以十六进制表示的整数
%o 无符号以八进制表示的整数
%g 把输出的值按照 %e 或者 %f 类型中输出长度较小的方式输出
%p 输出地址符
%lu 32位无符号整数
%llu 64位无符号整数

2.变量
基本类型 ： char int float double void

3.常量
转义符：
转义序列含义
\\\ 字符
\'' 字符
\"" 字符
\?? 字符
\a警报铃声
\b退格键
\f换页符
\n换行符
\r回车
\t水平制表符
\v垂直制表符
\ooo一到三位的八进制数
\xhh . . .一个或多个数字的十六进制数
定义常量：
#define 预处理器  //#define ONE 1
const  //const int var = 1;

4.存储类
auto:局部变量默认存储类
register:用于定义存储在寄存器中而不是 RAM 中的局部变量 最大尺寸等于寄存器大小，不能用 ‘&’取地址，因为没有在内存当中
static :修饰局部变量可以在函数调用之间保持局部变量的值。修饰全局变量时，会使变量的作用域限制在声明它的文件内。
extern:extern可以置于变量或者函数前，以标示变量或者函数的定义在别的文件中，提示编译器遇到此变量和函数时在其他模块中寻找其定义。此外extern也可用来进行链接指定。
场景一：extern "C" void fun(int a, int b);则告诉编译器在编译fun这个函数名时按着C的规则去翻译相应的函数名而不是C++的，C++的规则在翻译这个函数名时会把fun这个名字变得面目全非
场景二：extern 不与"C"在一起修饰变量或函数时，如在头文件中: extern int g_Int; 它的作用就是声明函数或全局变量的作用范围的关键字

sizeof:返回变量大小 字节
&var:返回变量地址 *解引用

判断 if else if   switch cace    ?: 和java一样
循环 while do while for 和java一样
函数： 和java一样//但是c里面的函数使用前需要先声明 int sum(int, int);
函数参数：
传值调用该方法把参数的实际值复制给函数的形式参数。在这种情况下，修改函数内的形式参数不会影响实际参数。
引用调用通过指针传递方式，形参为指向实参地址的指针，当对形参的指向操作时，就相当于对实参本身进行的操作。

作用域规则： 静态变量  局部变量 成员变量

5.C数组type arrayName [ arraySize ];
int balance[10];初始化： int balance[5] = {12,3,4,5}
多维数组type arrayName [ x ][ y ]
int arr[2,2] = {{12,23},{4,5}}
传递数组给函数(int *param)  (int param[10])(int param[])
double d[3] = {1.0135,2.0135,3.0135};
double *ptr_d = d;
访问每个元素  *(ptr_d + i)  或者 *(d + i)//两种方式等价

6.枚举(enum) 
方式一：
enum DAY {ONE = 2, TWO,THREE}; //enum DAY day = ONE;  
方式二：
#define ONE 1

7.函数指针与回调函数
typedef int (*fun_ptr)(int,int)
例 :
int max(int a,int b) {
return a > b ? a : b;
}
int (*ptr_max)(int,int) = &max;//可以写成 int (*ptr_max)(int,int) = max &可以省略，函数不需要括号
int max = ptr_max(10,ptr_max(20,50));//函数指针调用时就算是空参也需要加()
回调函数：就是将函数指针当作参数传递给其他函数

void populate_array(int *arr,int size,int (*getNextValue)(void)) {
    for (int i = 0; i < size; ++i) {
        *arr = getNextValue();
        arr++;
    }
}
int getNextRandomValue() {
    return rand();
}
void func1() {
    int array[10];
    populate_array(array,10,getNextRandomValue);
    
}

8.C字符串
在C语言里面字符串使用null字符'\0'终止一维字符数组,由于在数组末尾存储了空字符，所以数组的大小比字符长度+1
char site[7] ={'r','u','n','0','0','b','\0'}; = char site[] = "runoob";不需要把 null 字符放在字符串常量的末尾。C 编译器会在初始化数组时，自动把 \0 放在字符串的末尾
sizeof(site);//长度等于7  char name[] = "zhangjunpu"; sizeof(name) 长度等于11
字符串内存是连续的
C 中有大量操作字符串的函数 //需导入  #include <string.h>
序号函数 & 目的
strcpy(s1, s2);复制字符串 s2 到字符串 s1。
strcat(s1, s2);连接字符串 s2 到字符串 s1 的末尾。
strlen(s1);返回字符串 s1 的长度。
strcmp(s1, s2);如果 s1 和 s2 是相同的，则返回 0；如果 s1<s2 则返回小于 0；如果 s1>s2 则返回大于 0。
strchr(s1, ch);返回一个指针，指向字符串 s1 中字符 ch 的第一次出现的位置。
strstr(s1, s2);返回一个指针，指向字符串 s1 中字符串 s2 的第一次出现的位置。

9.结构体: 结构是 C 编程中另一种用户自定义的可用的数据类型，它允许您存储不同类型的数据项。
方式一：
struct Book{
    char title[50];
};
struct Book b1,b2;
方式二：
typedef struct {
    char name[50];
}Book;
Book b1,b2;
方式三：
struct Book{
    char name[50];
}book1,book2;
//use book1  book2
特殊情况：如果两个结构体互相包含，则需要对其中一个结构体进行不完整声明
struct B;//不完整声明
struct A {
struct B* ptr_b;
};
struct B {
struct A* ptr_a;
}
结构体变量初始化
struct Books
{
   char  title[50];
   char  author[50];
   char  subject[100];
   int   book_id;
} book = {"C 语言", "RUNOOB", "编程语言", 123456};
访问结构成员: 成员访问运算符(.) 
位域：一个字节中的二进位划分为几个不同的区域，并说明每个区域的位数。每个域有一个域名，允许在程序中按域名进行操作。 //类型说明符 位域名: 位域长度  位域长度不能大于1字节
1.剩下的位不够存储下一个位域时会从下一个字节开始存储 2.位域不允许跨字节 3.位域可空，做填充调整

struct {
    type [member_name] : width;    //unsigned int age :  3; 最多存储0-7 8会溢出
};
type    只能为 int(整型)，unsigned int(无符号整型)，signed int(有符号整型) 三种类型，决定了如何解释位域的值。
member_name    位域的名称。
width    位域中位的数量。宽度必须小于或等于指定类型的位宽度。

共同体和结构体类似，但是某一个时刻只使用一个变量,sizeof会得到最大空间的的字节

10.typedef 可以使用它来为类型取一个新的名字
typedef unsigned char BYTE;  BYTE b1,b2;

typedef vs #define
#define 是 C 指令，用于为各种数据类型定义别名，与 typedef 类似，但是它们有以下几点不同：

    typedef 仅限于为类型定义符号名称，#define 不仅可以为类型定义别名，也能为数值定义别名，比如您可以定义 1 为 ONE。
    typedef 是由编译器执行解释的，#define 语句是由预编译器进行处理的。

11.C 输入 & 输出
C 语言把所有的设备都当作文件。所以设备（比如显示器）被处理的方式与文件相同。以下三个文件会在程序执行时自动打开，以便访问键盘和屏幕。
标准文件        文件指针        设备
标准输入        stdin        键盘
标准输出        stdout        屏幕
标准错误        stderr        您的屏幕
C 语言中的 I/O (输入/输出) 通常使用 printf() 和 scanf() 两个函数。scanf() 函数用于从标准输入（键盘）读取并格式化， printf() 函数发送格式化输出到标准输出（屏幕）。

getchar() & putchar() 函数
int getchar(void) 函数从屏幕读取下一个可用的字符，并把它返回为一个整数。这个函数在同一个时间内只会读取一个单一的字符。您可以在循环内使用这个方法，以便从屏幕上读取多个字符。
int putchar(int c) 函数把字符输出到屏幕上，并返回相同的字符。这个函数在同一个时间内只会输出一个单一的字符。您可以在循环内使用这个方法，以便在屏幕上输出多个字符。

gets() & puts() 函数
char *gets(char *s) 函数从 stdin 读取一行到 s 所指向的缓冲区，直到一个终止符或 EOF。
int puts(const char *s) 函数把字符串 s 和一个尾随的换行符写入到 stdout。

scanf() 和 printf() 函数
int scanf(const char *format, ...) 函数从标准输入流 stdin 读取输入，并根据提供的 format 来浏览输入。//空格会算作终止符号
int printf(const char *format, ...) 函数把输出写入到标准输出流 stdout ，并根据提供的格式产生输出。

12.文件读写
打开文件：FILE *fopen( const char * filename, const char * mode );
关闭文件：int fclose( FILE *fp );                    最好是打开后就马上写关闭函数，避免忘记
写入文件：int fputc( int c, FILE *fp );    //写入字符
写入文件：int fputs( const char *s, FILE *fp );    //写入字符串
读取文件：int fgetc( FILE * fp );    //读取单个字符
读取文件：char *fgets( char *buf, int n, FILE *fp );//读取多个字符
/*
函数 fgets() 从 fp 所指向的输入流中读取 n - 1 个字符。它会把读取的字符串复制到缓冲区 buf，并在最后追加一个 null 字符来终止字符串。
如果这个函数在读取最后一个字符之前就遇到一个换行符 '\n' 或文件的末尾 EOF，则只会返回读取到的字符，包括换行符。您也可以使用 int fscanf(FILE *fp, const char *format, ...) 函数来从文件中读取字符串，但是在遇到第一个空格和换行符时，它会停止读取。
*/
filename 是字符串，用来命名文件，访问模式 mode 的值可以是下列值中的一个：
模式        描述
r        打开一个已有的文本文件，允许读取文件。
w        打开一个文本文件，允许写入文件。如果文件不存在，则会创建一个新文件。在这里，您的程序会从文件的开头写入内容。如果文件存在，则该会被截断为零长度，重新写入。
a        打开一个文本文件，以追加模式写入文件。如果文件不存在，则会创建一个新文件。在这里，您的程序会在已有的文件内容中追加内容。
r+        打开一个文本文件，允许读写文件。
w+        打开一个文本文件，允许读写文件。如果文件已存在，则文件会被截断为零长度，如果文件不存在，则会创建一个新文件。
a+        打开一个文本文件，允许读写文件。如果文件不存在，则会创建一个新文件。读取会从文件的开头开始，写入则只能是追加模式。

如果处理的是二进制文件，则需使用下面的访问模式来取代上面的访问模式：

"rb", "wb", "ab", "rb+", "r+b", "wb+", "w+b", "ab+", "a+b"

二进制 I/O 函数
下面两个函数用于二进制输入和输出：
size_t fread(void *ptr, size_t size_of_elements,
             size_t number_of_elements, FILE *a_file);
size_t fwrite(const void *ptr, size_t size_of_elements,
             size_t number_of_elements, FILE *a_file);
这两个函数都是用于存储块的读写 - 通常是数组或结构体。

13. C 预处理器
#define    定义宏
#include    包含一个源代码文件
#undef    取消已定义的宏
#ifdef    如果宏已经定义，则返回真
#ifndef    如果宏没有定义，则返回真
#if    如果给定条件为真，则编译下面代码
#else    #if 的替代方案
#elif    如果前面的 #if 给定条件不为真，当前条件为真，则编译下面代码
#endif    结束一个 #if……#else 条件编译块
#error    当遇到标准错误时，输出错误消息
#pragma    使用标准化方法，向编译器发布特殊的命令到编译器中

简单例子：
#undef  FILE_SIZE        //取消已定义的宏
#define FILE_SIZE 42    //定义宏
#ifndef MESSAGE                    //如果宏没有定义
   #define MESSAGE "You wish!"    //定义
#endif                            //结束
#ifdef DEBUG
   /* Your debugging statements here */
#endif
这个指令告诉 CPP 如果定义了 DEBUG，则执行处理语句。在编译时，如果您向 gcc 编译器传递了 -DDEBUG 开关量，这个指令就非常有用。它定义了 DEBUG，您可以在编译期间随时开启或关闭调试。

常用的已经定义的宏：
宏            描述
__DATE__    当前日期，一个以 "MMM DD YYYY" 格式表示的字符常量。
__TIME__    当前时间，一个以 "HH:MM:SS" 格式表示的字符常量。
__FILE__    这会包含当前文件名，一个字符串常量。
__LINE__    这会包含当前行号，一个十进制常量。
__STDC__    当编译器以 ANSI 标准编译时，则定义为 1。

预处理器运算符：
宏延续运算符（\）    //就是一行太长，写出来太丑，分成两行。。。
字符串常量化运算符（#）  在宏定义中，当需要把一个宏的参数转换为字符串常量时，则使用字符串常量化运算符（#）
#define  message_for(a, b)  \
    printf(#a " and " #b ": We love you!\n")
标记粘贴运算符（##）宏定义内的标记粘贴运算符（##）会合并两个参数。它允许在宏定义中两个独立的标记被合并为一个标记。
#define tokenpaster(n) printf("token" #n" = %d",token##n)    == printf ("token34 = %d", token34);
    int token34 = 40;
    tokenpaster(34);   //token505 = 689
defined() 运算符    预处理器 defined 运算符是用在常量表达式中的，用来确定一个标识符是否已经使用 #define 定义过。如果指定的标识符已定义，则值为真（非零）。如果指定的标识符未定义，则值为假（零）
    #if !defined (MESSAGE)
       #define MESSAGE "You wish!"
    #endif

参数化的宏 先定义#define指令    宏名称和左圆括号之间不允许有空格
    #define MAX(x, y) ((x)>(y)?(x):(y))

14.C 头文件
头文件是扩展名为 .h 的文件，包含了 C 函数声明和宏定义，被多个源文件中引用共享。有两种类型的头文件：程序员编写的头文件和编译器自带的头文件。
建议把所有的常量、宏、系统全局变量和函数原型写在头文件中        //#include 指令会指示 C 预处理器浏览指定的文件作为输入。
引用头文件的语法
#include <file>        用于引用系统头文件        //它在系统目录的标准列表中搜索名为 file 的文件。在编译源代码时，您可以通过 -I 选项把目录前置在该列表前。
#include "file"        用于引用用户头文件        //它在包含当前文件的目录中搜索名为 file 的文件。在编译源代码时，您可以通过 -I 选项把目录前置在该列表前
有条件引用：
#if SYSTEM_1
   # include "system_1.h"
#elif SYSTEM_2
   # include "system_2.h"
#elif SYSTEM_3
   ...
#endif
在有多个 .h 文件和多个 .c 文件的时候，往往我们会用一个 global.h 的头文件来包括所有的 .h 文件，然后在除 global.h 文件外的头文件中 包含 global.h 就可以实现所有头文件的包含，同时不会乱。方便在各个文件里面调用其他文件的函数或者变量。
#ifndef _GLOBAL_H
#define _GLOBAL_H
#include <fstream>
#include <iostream>
#include <math.h>
#include <Config.h>

15.强制类型转换        //跟java一样强转

16.错误处理    ？？？？？？？

17.递归 //跟java一样
double factorial(int i) {
    if (i == 1) {
        return 1;
    }
    return i * factorial(i - 1);
}

18.可变参数    stdarg.h
    在函数定义中创建一个 va_list 类型变量，该类型是在 stdarg.h 头文件中定义的。
    使用 int 参数和 va_start 宏来初始化 va_list 变量为一个参数列表。宏 va_start 是在 stdarg.h 头文件中定义的。
    使用 va_arg 宏和 va_list 变量来访问参数列表中的每个项。
    使用宏 va_end 来清理赋予 va_list 变量的内存。
double average(int num,...) {
    va_list vaList;
    va_start(vaList,num);    //为 num 个参数初始化 valist
    va_arg(valist, int);    // 访问所有赋给 valist 的参数
    va_end(valist);             //清理为 valist 保留的内存
}

19.内存管理
序号函数和描述
1void *calloc(int num, int size);
在内存中动态地分配 num 个长度为 size 的连续空间，并将每一个字节都初始化为 0。所以它的结果是分配了 num*size 个字节长度的内存空间，并且每个字节的值都是0。
2void free(void *address);
该函数释放 address 所指向的内存块,释放的是动态分配的内存空间。
3void *malloc(int num);
在堆区分配一块指定大小的内存空间，用来存放数据。这块内存空间在函数执行完成后不会被初始化，它们的值是未知的。
4void *realloc(void *address, int newsize);
该函数重新分配内存，把内存扩展到 newsize。

20.命令行参数
执行程序时，可以从命令行传值给 C 程序。这些值被称为命令行参数//
int main(int argc, char* argv[]);//arguments count; arguments getNextValu  argc 至少是1
重点: argv[0] 存储程序的名称，argv[1] 是一个指向第一个命令行参数的指针，*argv[n] 是最后一个参数

21.C标准库
<string.h>
size_t 这是无符号整数类型，它是 sizeof 关键字的结果。

----------------------------------------------------------C++内容------------------------------------------------------------------------------------------
1.C++类和对象
析构函数:析构函数是类的一种特殊的成员函数，它会在每次删除所创建的对象时执行。析构函数有助于在跳出程序（比如关闭文件、释放内存等）前释放资源。
拷贝构造函数:它在创建对象时，是使用同一类中之前创建的对象来初始化新创建的对象。  //如果在类中没有定义拷贝构造函数，编译器会自行定义一个。如果类带有指针变量，并有动态内存分配，则它必须有一个拷贝构造函数
  通过使用另一个同类型的对象来初始化新创建的对象。
  复制对象把它作为参数传递给函数。
  复制对象，并从函数返回这个对象。
Box (const Box &obj) {
  //构造函数主体
}
友元函数:类的友元函数是定义在类外部，但有权访问类的所有私有（private）成员和保护（protected）成员。
class Box
{
   double width;
public:
   double length;
   friend void printWidth( Box box ); //友元函数
   friend class BBox;
   void setWidth( double wid );
};
void printWidth(Box box) {
    cout << "printWidth# width: " << box.width << endl; //直接box.width  就是这么牛批
}
内联函数(inline):内联函数是通常与类一起使用。如果一个函数是内联的，那么在编译时，编译器会把该函数的代码副本放置在每个调用该函数的地方。//几行的函数可以定义成内联函数
:: 范围解析运算符 

2.继承  class derived-class: access-specifier base-class  //和java的继承访问权限一样
class Animal{

}
class Dog : public Animal{

}
访问        public      protected     private
同一个类      yes           yes         yes
派生类        yes           yes         no
外部的类      yes           no          no
多继承
class <派生类名>:<继承方式1><基类名1>,<继承方式2><基类名2>,…
{
<派生类类体>
};

3.重载运算符重载函数
Box operator+(const Box&);
可重载运算符/不可重载运算符
可重载的运算符列表：
双目算术运算符 + (加)，-(减)，*(乘)，/(除)，% (取模)
关系运算符 ==(等于)，!= (不等于)，< (小于)，> (大于)，<=(小于等于)，>=(大于等于)
逻辑运算符 ||(逻辑或)，&&(逻辑与)，!(逻辑非)
单目运算符 + (正)，-(负)，*(指针)，&(取地址)
自增自减运算符 ++(自增)，--(自减)
位运算符  | (按位或)，& (按位与)，~(按位取反)，^(按位异或),，<< (左移)，>>(右移)
赋值运算符 =, +=, -=, *=, /= , % = , &=, |=, ^=, <<=, >>=
空间申请与释放 new, delete, new[ ] , delete[]
其他运算符 ()(函数调用)，->(成员访问)，,(逗号)，[](下标)

不可重载的运算符列表：
.：成员访问运算符
.*, ->*：成员指针访问运算符
::：域运算符
sizeof：长度运算符
?:：条件运算符
#： 预处理符号

4.多态
虚函数(virtual):静态多态，或静态链接 - 函数调用在程序执行前就准备好了。有时候这也被称为早绑定，只需要在父类的方法前面加上virtual关键字就可以
纯虚函数:虚函数 是在基类中使用关键字 virtual 声明的函数。在派生类中重新定义基类中定义的虚函数时，会告诉编译器不要静态链接到该函数。这种操作被称为动态链接，或后期绑定。
class Parent {
  virtual print() {}  //virtual虚函数
  virtual pura() = 0; //  = 0 告诉编译器，函数没有主体，上面的虚函数是纯虚函数。强制子类重写
}

5.接口(抽象类) : 也是接用纯虚函数让子类强制重写方法...

6.C++文件和流
数据类型  描述
ofstream  该数据类型表示输出文件流，用于创建文件并向文件写入信息。
ifstream  该数据类型表示输入文件流，用于从文件读取信息。
fstream 该数据类型通常表示文件流，且同时具有 ofstream 和 ifstream 两种功能，这意味着它可以创建文件，向文件写入信息，从文件读取信息。
模式标志  描述
ios::app  追加模式。所有写入都追加到文件末尾。
ios::ate  文件打开后定位到文件末尾。
ios::in 打开文件用于读取。
ios::out  打开文件用于写入。
ios::trunc  如果该文件已经存在，其内容将在打开文件之前被截断，即把文件长度设为 0。

const char* file = "aaaaaaaa.txt";
char buffer[100];
ifstream  infile;
infile.open(file);
infile >> buffer;   //outfile << buffer; 写入到文件
cout << "file: " << buffer << endl;
infile.close();

istream 和 ostream 都提供了用于重新定位文件位置指针的成员函数。这些成员函数包括关于 istream 的 seekg（"seek get"）和关于 ostream 的 seekp（"seek put"）
// 定位到 fileObject 的第 n 个字节（假设是 ios::beg）
fileObject.seekg( n );
// 把文件的读指针从 fileObject 当前位置向后移 n 个字节
fileObject.seekg( n, ios::cur );
// 把文件的读指针从 fileObject 末尾往回移 n 个字节
fileObject.seekg( n, ios::end );
// 定位到 fileObject 的末尾
fileObject.seekg( 0, ios::end );

7.C++异常处理
异常  描述
std::exception  该异常是所有标准 C++ 异常的父类。
std::bad_alloc  该异常可以通过 new 抛出。
std::bad_cast 该异常可以通过 dynamic_cast 抛出。
std::bad_exception  这在处理 C++ 程序中无法预期的异常时非常有用。
std::bad_typeid 该异常可以通过 typeid 抛出。
std::logic_error  理论上可以通过读取代码来检测到的异常。
std::domain_error 当使用了一个无效的数学域时，会抛出该异常。
std::invalid_argument 当使用了无效的参数时，会抛出该异常。
std::length_error 当创建了太长的 std::string 时，会抛出该异常。
std::out_of_range 该异常可以通过方法抛出，例如 std::vector 和 std::bitset<>::operator[]()。
std::runtime_error  理论上不可以通过读取代码来检测到的异常。
std::overflow_error 当发生数学上溢时，会抛出该异常。
std::range_error  当尝试存储超出范围的值时，会抛出该异常。
std::underflow_error  当发生数学下溢时，会抛出该异常。
try{

}catch (const char* msg) {

}

8.C++动态内存
C++ 程序中的内存分为两个部分:
栈：在函数内部声明的所有变量都将占用栈内存。
堆：这是程序中未使用的内存，在程序运行时可用于动态分配内存。

new 和 delete 运算符
int* ptr_i = NULL;
ptr_i = new int;
delete ptr_i;
C++ 也可以使用C的malloc函数 但是:new 与 malloc() 函数相比，其主要的优点是，new 不只是分配了内存，它还创建了对象。

数组动态内存分配
char* pvalue  = NULL;   // 初始化为 null 的指针
pvalue  = new char[20]; // 为变量请求内存
delete [] pvalue;        // 删除 pvalue 所指向的数组
C++一维,二维,三维数组动态内存分配   //参考https://www.runoob.com/cplusplus/cpp-dynamic-memory.html

9.命名空间
命名空间的定义使用关键字 namespace，后跟命名空间的名称。可作为附加信息来区分不同库中相同名称的函数、类、变量等。
using namespace 指令，这样在使用命名空间时就可以不用在前面加上命名空间的名称。这个指令会告诉编译器，后续的代码将使用指定的命名空间中的名称。
namespace namespace_name {
   // 代码声明
}
use:  //命名空间可以嵌套  访问方式:
zhang::age
zhang::func1();       //单级
zhang::jun::func1();  //多级

10.模板   //就是java里的泛型方法和泛型类......
函数模板:
template <typename type> ret-type func-name(parameter list) {
   // 函数的主体
}
模板方法例子:
template <typename T>
inline T const& Max (T const& a, T const& b) { 
    return a < b ? b:a; 
} 
模板类:
template <class type> class class-name {
}
模板类例子:
template<class T>
class Stack {
private:
    vector<T> elements;
public:
    void push(const T &t) {
        elements.push_back(t);
    }

    T pop() {
        if (elements.empty()) {
            throw out_of_range("elements is empty");
        }
        return elements.pop_back();
    }
};

11.C++预处理器
#define PI 3.1415926
#ifndef MY_NULL
    #define MY_NULL 5200
#endif
//再比如定义C++类的头文件
#ifndef UNTITLED1_BOX_H //如果没有定义
#define UNTITLED1_BOX_H //定义
class Box{
  //....
}
#endif //UNTITLED1_BOX_H  //定义结束
C++预定义处理器:
宏 描述
__LINE__  这会在程序编译时包含当前行号。       //6 
__FILE__  这会在程序编译时包含当前文件名。      //test.cpp
__DATE__  这会包含一个形式为 month/day/year 的字符串，它表示把源文件转换为目标代码的日期。  //Feb 28 2011
__TIME__  这会包含一个形式为 hour:minute:second 的字符串，它表示程序被编译的时间。      //18:52:48

12.C++信号处理
信号是由操作系统传给进程的中断，会提早终止一个程序。在 UNIX、LINUX、Mac OS X 或 Windows 系统上，可以通过按 Ctrl+C 产生中断。
有些信号不能被程序捕获，但是下表所列信号可以在程序中捕获，并可以基于信号采取适当的动作。这些信号是定义在 C++ 头文件 <csignal> 中。
信号        描述
SIGABRT     程序的异常终止，如调用 abort。
SIGFPE      错误的算术运算，比如除以零或导致溢出的操作。
SIGILL      检测非法指令。
SIGINT      程序终止(interrupt)信号。
SIGSEGV     非法访问内存。
SIGTERM     发送到程序的终止请求。

signal() 函数   用来捕获突发事件。
void (*signal (int sig, void (*func)(int)))(int);   //这么恶心的函数 就是一个函数指针
void signalHandler( int signum )
{
    cout << "Interrupt signal (" << signum << ") received.\n";
    // 清理并关闭
    // 终止程序 
   exit(signum);  
 
}
int main(int argc,char *argv[]) {
    signal(SIGINT, signalHandler);
    return 0;
}

raise() : 可以使用函数 raise() 生成信号，该函数带有一个整数信号编号作为参数 //简单来说就是终止不用Ctrl+C了,直接给个条件满足了就调用 raise(SIGINT); 跟Ctrl+C是一个效果

13.C++多线程
#include <pthread.h>
pthread_create (thread, attr, start_routine, arg);  //创建线程
参数           描述
thread        指向线程标识符指针。
attr          一个不透明的属性对象，可以被用来设置线程属性。您可以指定线程属性对象，也可以使用默认值 NULL。
start_routine 线程运行函数起始地址，一旦线程被创建就会执行。
arg           运行函数的参数。它必须通过把引用作为指针强制转换为 void 类型进行传递。如果没有传递参数，则使用 NULL。
pthread_exit (status);  //退出线程 pthread_exit 用于显式地退出一个线程。通常情况下，pthread_exit() 函数是在线程完成工作后无需继续存在时被调用。如果 main() 是在它所创建的线程之前结束，并通过 pthread_exit() 退出，那么其他线程将继续执行。否则，它们将在 main() 结束时自动被终止。
//下面这段代码演示了如何向线程传递参数
struct thread_data{
   int  thread_id;
   char *message;
};
 
void *PrintHello(void *threadarg)
{
   struct thread_data *my_data;
   my_data = (struct thread_data *) threadarg;
   cout << "Thread ID : " << my_data->thread_id ;
   cout << " Message : " << my_data->message << endl;
   pthread_exit(NULL);
}
 
int main ()
{
   pthread_t threads[NUM_THREADS];
   struct thread_data td[NUM_THREADS];
   int rc;
   int i;
 
   for( i=0; i < NUM_THREADS; i++ ){
      cout <<"main() : creating thread, " << i << endl;
      td[i].thread_id = i;
      td[i].message = (char*)"This is message";
      rc = pthread_create(&threads[i], NULL,
                          PrintHello, (void *)&td[i]);
      if (rc){
         cout << "Error:unable to create thread," << rc << endl;
         exit(-1);
      }
   }
   pthread_exit(NULL);
}

连接和分离线程
pthread_join (threadid, status) 
pthread_detach (threadid)
// 初始化并设置线程为可连接的（joinable）
pthread_attr_init(&attr);
pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
// 删除属性，并等待其他线程
pthread_attr_destroy(&attr);

13.STL
C++ 标准模板库的核心包括以下三个组件：
组件                        描述
容器（Containers）    容器是用来管理某一类对象的集合。C++ 提供了各种不同类型的容器，比如 deque、list、vector、map 等。
算法（Algorithms）    算法作用于容器。它们提供了执行各种操作的方式，包括对容器内容执行初始化、排序、搜索和转换等操作。
迭代器（iterators）   迭代器用于遍历对象集合的元素。这些集合可能是容器，也可能是容器的子集。
vector:
push_back( )  成员函数在向量的末尾插入值，如果有必要会扩展向量的大小。
size( )       函数显示向量的大小。
begin( )      函数返回一个指向向量开头的迭代器。
end( )        函数返回一个指向向量末尾的迭代器。C++ STL 之 vector 的 capacity 和 size 属性区别:size 是当前 vector 容器真实占用的大小，也就是容器当前拥有多少个容器。
capacity 是指在发生 realloc 前能允许的最大元素数，即预分配的内存空间。
这两个属性分别对应两个方法：resize() 和 reserve()。使用 resize() 容器内的对象内存空间是真正存在的。使用 reserve() 仅仅只是修改了 capacity 的值，容器内的对象并没有真实的内存空间(空间是"野"的)。//分配了空间但是空间里面没内容

14.C++标准库 
C++ 标准库可以分为两部分：
标准函数库：       这个库是由通用的、独立的、不属于任何类的函数组成的。函数库继承自 C 语言。
面向对象类库：     这个库是类及其相关函数的集合。标准函数库

标准函数库分为以下几类：
输入/输出 I/O
字符串和字符处理
数学
时间、日期和本地化
动态分配
其他
宽字符函数
标准函数库

面向对象类库
标准的 C++ 面向对象类库定义了大量支持一些常见操作的类，比如输入/输出 I/O、字符串处理、数值处理。面向对象类库包含以下内容：
标准的 C++ I/O 类
String 类
数值类
STL 容器类
STL 算法
STL 函数对象
STL 迭代器
STL 分配器
本地化库
异常处理类
杂项支持库

结尾:
C++ 有用的书籍
《Essential C++ 中文版》
《C++ Primer Plus 第6版中文版》
《C++ Primer中文版（第5版）》
有用的网站:
C++ 标准库:  https://en.cppreference.com/w/cpp/header
```

