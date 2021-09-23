## 概述

### 前言

**gcc & clang**

gcc是GNU Compiler Collection，也称为GNU编译器套件，可以理解为编译器，它可以编译很多种语言：C/C++、FORTRAN、JAVA、OBJC、ADA等。假如我有一个源文件（比如.c文件），我就可以用gcc命令来编译它。



clang也一样，Clang是一个C语言、C++、Objective-C、Objective-C++语言的轻量级编译器。源代码发布于BSD协议下。Clang将支持其普通lambda表达式、返回类型的简化处理以及更好的处理constexpr关键字。

clang和gcc相比，比gcc编译速度更快一些，而且错误提示更人性化。

**make makefile cmake CMakeList**

**什么是make?**

**make是一个命令工具, 是一个解释makefile中指令的命令工具.**make工具相当于一个智能的批处理工具，本身没有编译和链接的功能，而是用类似于批处理的方式通过调用makefile文件中用户指定的命令来进行编译和链接。

**makefile**

用户自己定义的一套编译规则

**cmake & CMakeList**

**cmake工具可以更加方便地生成makefile文件给make用。cmake还有其他功能，就是**可以跨平台生成对应平台能用的makefile，无需自己根据每个平台的不同特性去修改**。而**cmake根据什么生成makefile，它需要根据CMakeLists.txt文件（组态档）去生成makefile。

**ninja gn?**