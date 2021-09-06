## Kotlin语法笔记

```kotlin
--------------------------------------------------------------
val:value 不可变引用 对应java final
var:variable 可变引用 
类？：把数据和处理数据的代码封装成一个单一的实体
--------------------------------------------------------------
enum class Color{
    RED,BLUE,YELLOW,GREEN
}

enum class Color(val r:Int, val g:Int  val b:Int) {
    RED(255,0,0), GREEN(0,255,0),BLUE(0,0,255)
}
--------------------------------------------------------------
for(i in 100 downTo 1 step 2)  downTo setp  until 

遍历map：
val binaryReps = TreeMap<Char,String>()
for ((letter,binary) in binaryReps)

val list = listOf("zhangsan","lisi")
for ((index, element) in list.withIndex())

in :  可以检查区间的成员
例： fun isLatter(c: Char) = c in 'a'..'z'
--------------------------------------------------------------
第三章 函数的定义与调用
创建集合：
val set = hashSetOf(12,13,145)
val list = arrayListOf(12,13,15)
val map = hashMapOf(1 to "one", 2 to "two")

参数默认值 fun <T> joinToString(seperator:String = ", ",prefix: String = "",suffix: String = "")
调用： joinToString(list,suffix ="   ",prefix = "  ")

@JvmOverloads   //指示编译器生成java重载函数，从最后一个开始省略每个参数
fun <T> prettyPrint(src:Collection<T>,separator:String,prefix:String,suffix:String) {
        val sb = StringBuffer(prefix)
        for ((index, element) in src.withIndex()) {

            sb.append(element)
            if (index != src.size -1) {
                sb.append(separator)
            }
        }
        sb.append(suffix)
        println(sb.toString())
}

//比如原来的文件名的Join.kt 在java类里面调用时用Join.pretty2String()
//如果通过下面的注解，在java类里面变成了StringFunctions.pretty2String()
@file:JvmName("StringFunctions")    
package com.example.ktstudy.strings
fun pretty2String():String{return ""}

//可以给String添加方法，在外面调用如下："abc".lastChar()
fun String.lastChar() :Char =this[length - 1]

vararg： 可变参数
mapOf(1 to "one"): 1 to "one" = 1.to("one")
"""(.+)/(.+)\.(.+)""".toRegex() //""" 里面的任何字符都不会被转义，包括反斜线
--------------------------------------------------------------
chapter 4 类、对象和接口
接口定义：
interface Clickable{
    fun click()     //强制重写
    fun sayHello() = println("hello")   //kt 允许默认实现方法 这样可以不用强制子类重写
}

**kt中类以及类的方法默认时final如果想要继承，需要显示将父类改为open 方法亦是如此   //open class parent
**抽象方法遵循java规则： 抽象函数必须是抽象类
**java中class里面写class嵌套的class是默认持有外部类引用，kt中相当于静态类，如果想变成内部类需要inner关键字：
class Outer{
    inner class Inner{

    }
}
！！！kt初始化类：主构造方法与初始化语句块
class User(val name:String) {//主构造
    init{   //初始化代码块  仅次于主构造    类被创建时执行

    }
} 
class User constructor(_nickName: String) //这个constructor可以省略

class User(val name:String)     //下面这两种方式等价 但是第一种更加简洁  这种相当于熟悉会直接通过构造方法初始化
class User(_name:String){       //
    val name = _name
}

class User(val name: String = "zjp")    //构造方法可以声明一个默认值

//创建对象
val user = User("zjp")

继承：
//有参数继承方式：
open class A(val name:String)   
class B(name: String) :A(name)  //如果存在继承关系，必须要初始化父类构造

//无参数继承方式：
open class A
class B :A()        //必须显示的调用父类的构造方法，即使是空参

//不想被继承
class A private constructor(){}

不同的方式初始化父类： (这种需要重点记住)
open class MyButton:View {
    constructor(ctx:Context):super(ctx){}     或者  constructor(ctx:Context):this(ctx,null) {}

    constructor(ctx:Context,attr: AttributeSet):super(ctx,attr)
}

setter  getter

class A{
    var address : String = "haha"
        set(value) {
            field = value           //field为特殊标识符，可以访问这个address这个字段
        }

    var age:Int = 0
        private set                 //这样就不能在外部使用setter方法
}

a is A = java：  a instanceof A  

data类可以让编译器生成若干标准方法 //equals  hashCode toString 

by 关键字：委托
class MutableProxy<T>(inner:MutableList<T> = mutableListOf()) :MutableList<T> by inner
class CollectionsProxy<T>(inner:Collection<T> = ArrayList()) :Collection<T> by inner
val proxy = MutableProxy<String>()
proxy.add("ahaha")
proxy.remove("haha")
上面实现可以让这些proxy类具备实现了接口的能力，但是又不用自己去实现这些方法

object：该关键字定义一个类并同时创建一个实例。使用场景：
        对象声明
        伴生对象 companion
        对象表达式

--------------------------------------------------------------
chapter 5 Lambda编程
{x: Int, y: Int -> x + y}   //   参数 -> 函数体

kt中有这样一种语法规定，如果lambda表达式是函数调用的最后一个实参，它可以放到括号外面  people.maxBy(){p: Person -> p.age}
                      当lambda函数是唯一实参时，可以去掉代码中的空括号对  people.maxBy{p: Person -> p.age}

filter map  all any count find groupBy->map  flatMap flatten
map filter 都会创建两个两个临时集合来保存变量
优化：people.asSequence().map(Person::name).filter{it.startWith("A")}.toList()
当调用末端操作时：会一次对每个元素执行map-filter的操作而非 先map所有，再filter

OnClickListener接口只有一个抽象方法，这种接口被称为函数式接口或者SAM接口（SingleAbstractMethod）

with  apply
fun alphabeat(): String{
    val sb = StringBuffer()
    return with(sb) {    //接受两个参数的函数
        for(latter in 'A'..'Z'){
            this.append(latter)
        }
        this.toString()
    }
}

apply 与 with 基本一样： apply始终会返回作为实参传递给它的对象

fun alphabeat() = StringBuffer().apply {
    for(latter in 'A'..'Z' ) {
        append(latter)
    }
}.toString

--------------------------------------------------------------
chapter 6 kt类型系统
  ?.  ?:  as? !! let lateinit
val name：String ？= null
name?.let { it
    //能执行到这里说明name肯定不为空
}

kt数据类型：
基本 Int  Boolean Float Double Char ...
可空 Int? Boolean? ...
类型转换：
val i = 1
val l = i.toLong()
Any  Any? 相当于java里面的Object
Unit : kotlin 里面的 'void'
fun f(): Unit {}   =>  fun f() {}   //unit作为返回值是可以不写的
集合与数组：
MutableCollection &  Collection

集合类型                只读                      可变
List                  listOf            mutableListOf,arrayListOf
Set                   setOf             mutableSetOf,hashSetOf,linkedSetOf,sortedSetOf
Map                   mapOf             mutableMapOf,hashMapOf,linkedMapOf,sortedMapOf

kt创建数组： arrayOf  arrayOfNulls
val latters = Array(26) {('a' + it).toString()} //创建字符串数组 a-z
IntArray =  intArrayOf  //等价
--------------------------------------------------------------
chapter7 运算符重载及其他约定
7.1运算符重载
class Point(private val x : Int, private val y: Int) {
    operator fun plus(other: Point): Point {
        return Point(x + other.x, y + other.y)
    }
}
a * b = times       +a          unaryPlus
a / b = div         -a          unaryMinus
a % b = mod         !a          not
a + b = plus        ++a, a++    inc
a - b = minus       --a, a--    dec

shl(带符号左移) shr(带符号右移) ushr(无符号左移) and(按位与) or(按位或) xor(按位异或) inv(按位取反)
in -> contains
.. -> rangeTo
val now = LocalDate.now()
val vacation = now..now.plusDays(10)  => now.rangeTo(now.plusDays(10))  //创建一个区间
println(now.plusWeeks(1) in vacation)   //检测特定的时间是否属于这个区间

解构声明与组件函数
class Point(private val x : Int, private val y: Int) {
    operator fun plus(other: Point): Point {
        return Point(x + other.x, y + other.y)
    }
    operator fun component1() = x
    operator fun component2() = y
}
val point = Point(2, 3)
val (x,y) = point
val component1 = point.component1()
val component2 = point.component2()       //解构声明简单来说就是反推回去
使用场景：从一个函数返回多个值 sample:
data class NameComponents(val fullName: String, val extentions: String )
fun splitFileName(fullName :String) {
    val result = fullName.split('.', litmit = 2)
    return NameComponents(resut[0],result[1])
}

val (name, ext) = splitFileName("创业维艰.txt") //拆分
for ((key,value) in map) {          //例子2
    println("$key $value")
}

***委托属性***
by:委托给其他任何有set get方法的对象/属性  by lazy()
class Student {
    private val _attribute = hashMapOf<String, String>()
    var serial: String by _attribute
}
val intentValue : String by lazy {
    intent.getString(key)
}
--------------------------------------------------------------
chapter 8 高阶函数
高阶函数： 任何以lambda或者函数引用作为参数的函数，或者返回值为lambda或函数引用的函数，或者两者都满足的函数都是高阶函数
val sum = {x: Int, y: Int -> x + y} = (Int, Int) -> Int = { x,y -> x + y}

(Int, String)->Unit //参数类型  -> 返回类型
//函数参数 例子1
fun String.filter(predicate: (Char) -> Boolean): String {
    val ret = StringBuffer()
    for (index in 0 until length) {
        val ch = elementAt(index)
        if (predicate(ch)) {
            ret.append(ch)
        }
    }
    return ret.toString()
}       //use  "我是zhongguoren".filter { it -> it in 'a'..'z' }
//函数参数 例子2
fun <T> Collection<T>.joinToString(separator: String =", ",prefix = "",suffix="",transform:(<T> -> String)? = null) {
    val buffer = StringBuffer()
    for ((index,element) in this.withIndex) {
        if (index >0) buffer.append(separator)
        val str = transform?.invoke(element) ?: element.joinToString //兼容性更强
        buffer.append(str)
    }
    return buffer.toString()
}

高阶函数去除重复代码：
enum class OS {
    WINDOWS, LINUX, MAC, IOS, ANDROID
}
data class SiteVisit(val path: String, val duration: Float, val os: OS)
//需求是显示来自某一机器的平均访问时间
fun List<SiteVisit>.averageDurationFor(os : OS) =  filter { it.os == os }.map(SiteVisit::duration).average()

--------------------------------------------------------------
泛型：
协变：保留子类信息
open class Animal
class Cat : Animal()
class Herd<out T : Animal>  //
out含义：
    1.子类型化会保留（A<Cat> 是 A<Animal>的子类型）
    2.T只能用在out位置
逆变： 
in
--------------------------------------------------------------
注解与反射

@JvmName 将改变由kt生成的java方法或字段名称
@JvmStatic 能被用在对象声明或者伴生对象的方法上，把它们暴露成静态方法
@JvmOverloads 指导kt编译器为带默认参数值的函数生成多个重载函数 //从最后一个开始省略，直到省略到没有包含默认值的参数
@JvmField 可以应用于一个属性，把这个属性暴露成没有访问器的公有java字段

--------------------------------------------------------------
--------------------------------------------------------------

--------------------------------------------------------------









object:
internal: 只在内部模块可见
sealed: 所有直接子类必须嵌套在父类中

关于类：
如果你的类具有一个父类，主构造方法同样需要初始化父类，可通过基类列表的父类引用中提供父类构造方法初始化
class A(name:String) {//并非类的成员变量，只算初始化参数
    val name:String = name //这样赋值可行
    init{   //初始化代码块，会在构造类的时候执行

    }
}   
简化后：
class A(val name : String)

关于继承：
class B(name: String) : A(name) //必须初始化父类
或者
class B(val userName:String) : A(userName) //ok
class B(val name:String) : A(name)  //error  因为父类A有成员属性name  而B如此声明表示自己的成员属性也是叫name，这样造成冲突

如果没有给一个类生成任何的构造方法，将会生成一个不做任何事的构造方法：
open class Button()
class RichButton : Button() //必须显示调用父类构造方法
接口没有构造方法，所以实现接口的时候不需要在父类型列表后加上()    //主要还是区别父类是接口还是类
//自定义控件的构造方法如下
class CustomView:View {
    constructor(ctx: Context) :this(ctx,null)
    constructor(ctx: Context, attr: AttributeSet?):super(ctx,attr)
}

关键字：
by：委托实现


Lambda表达式：
kt中有这样一种语法规定，如果lambda表达式是函数调用的最后一个实参，它可以放到括号外面
val listOf = listOf(G("lisi", 18), G("lisi", 19), G("lisi", 20), G("lisi", 22))
listOf.maxBy{it.age} ==  listOf.maxBy(){}

集合操作：
all,any,count,find
count > size ： size会创建一个集合出来存储满足条件的元素，count只跟踪匹配元素数量



6.1可控性
?.  ?:   as?  !!   let
6.3集合与数组
kt集合实现分别为两个接口Collection（不具备添加或移除元素的功能） 和 MutableCollection  //只读 和 可写
```

