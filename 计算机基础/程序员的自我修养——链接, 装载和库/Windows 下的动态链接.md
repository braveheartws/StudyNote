### Windows 下的动态链接

#### 9.1 DLL简介

DLL即**动态链接库(Dynamic-LInk Library)**,DLL和EXE文件是一个概念,都是PE格式的二进制文件,稍有不同的是PE文件头部中有个符号位表示该文件是EXE或者事ELL

##### 9 .1.2 基地址和RVA

当一个PE文件被装载时,器进程地址中的其实地址就是**基地址(Base Address)**,每个PE文件都有一个优先装载的基地址. 这个值就是PE文件头的Image Base. 如果基地址已经被占用了,PE装载器会选用其他空闲地址.**相对地址(RVA, Relative Virtual Address)**就是基于基地址的偏移量

```
cl /LDd Math.c		//生成Debug版DLL
cl /LD Math.c		//生成Release版DLL
cl  Math.c			//生成.EXE可执行文件
```

```
dumpbin /EXPORTS Math.dll // 查看导出符号
```

##### 9.1.6 使用DLL

![](C:\Users\39535\Desktop\workspace\StudyNote\计算机基础\程序员的自我修养——链接, 装载和库\84.png)

![](C:\Users\39535\Desktop\workspace\StudyNote\计算机基础\程序员的自我修养——链接, 装载和库\85.png)

##### 9.1.8 DLL 显式运行时链接

* LoadLibrary: 装载一个DLL到进程的地址空间
* GetProcAddress: 查找某个符号地址
* FreeLibrary: 用来卸载某个已加载的模块

#### 9.2 符号导出表

##### 9.2.1 导出表

当一个PE需要将一些函数或者变量提供给其他PE文件使用时,这种行为称为**符号导出(Symbol Exporting)**, 所有导出的符号被集中存放在了被称作**导出表(Export Table)**的结构中.

![](C:\Users\39535\Desktop\workspace\StudyNote\计算机基础\程序员的自我修养——链接, 装载和库\86.png)

##### 9.2.3 导出重定向

```
HeapAlloc = NTDLL.RtAllocHeap // 将某个符号重定向到另外一个DLL
```

##### 9.2.4 导入表

某个程序依赖其他DLL 的函数或者变量:**符号导入(Symbol Importing)**

```
dumpbin /IMPORTS Math.dll   //查看模块依赖那些DLL,又导入了哪些函数
```

![](C:\Users\39535\Desktop\workspace\StudyNote\计算机基础\程序员的自我修养——链接, 装载和库\87.png)

.page