### ES6

#### 一 ES6简介

1.0	ECMAScript 6.0（以下简称 ES6）是 JavaScript 语言的下一代标准，已经在 2015 年 6 月正式发布了。它的目标，是使得 JavaScript 语言可以用来编写复杂的大型应用程序，成为企业级开发语言。

1.1	ECMAScript 和 JavaScript 的关系
要讲清楚这个问题，需要回顾历史。1996 年 11 月，JavaScript 的创造者 Netscape 公司，决定将 JavaScript 提交给标准化组织 ECMA，希望这种语言能够成为国际标准。次年，ECMA 发布 262 号标准文件（ECMA-262）的第一版，规定了浏览器脚本语言的标准，并将这种语言称为 ECMAScript，这个版本就是 1.0 版。

该标准从一开始就是针对 JavaScript 语言制定的，但是之所以不叫 JavaScript，有两个原因。一是商标，Java 是 Sun 公司的商标，根据授权协议，只有 Netscape 公司可以合法地使用 JavaScript 这个名字，且 JavaScript 本身也已经被 Netscape 公司注册为商标。二是想体现这门语言的制定者是 ECMA，不是 Netscape，这样有利于保证这门语言的开放性和中立性。

因此，ECMAScript 和 JavaScript 的关系是，前者是后者的规格，后者是前者的一种实现（另外的 ECMAScript 方言还有 JScript 和 ActionScript）。日常场合，这两个词是可以互换的。

1.2 ES6 与 ECMAScript 2015 的关系
ES6 既是一个历史名词，也是一个泛指，含义是 5.1 版以后的 JavaScript 的下一代标准，涵盖了 ES2015、ES2016、ES2017 等等，而 ES2015 则是正式名称，特指该年发布的正式版本的语言标准。本书中提到 ES6 的地方，一般是指 ES2015 标准，但有时也是泛指“下一代 JavaScript 语言”。

#### 二 基础语法

##### 2.1 let 和 const

**2.1.1 let命令**

声明和`var`类似，但是所声明的变量，只在`let`命令所在的代码块内有效。

使用场景

```javascript
for (let i = 0; i < 10; i++)
```

* `var`命令会发生变量提升现象,即违背声明前使用那么值为`undifined`,`let`修正了这个问题

**2.1.2 const命令**

***`const`声明一个只读的常量。一旦声明，常量的值就不能改变。**

* const修饰的内存地址,内存地址不能改变,但是可以改变内存地址的值(如引用对象)

---



**2.2数组的解构和赋值**

定义ES6 允许按照一定模式，从数组和对象中提取值，对变量进行赋值，这被称为解构（Destructuring）。

```javascript
let a = 1;
let b = 2;
let c = 3;
let [a,b,c] = [1,2,3]	//两种方式相等
```

* 本质上，这种写法属于“模式匹配”，只要等号两边的模式相同，左边的变量就会被赋予对应的值
* 只要某种数据结构具有 Iterator 接口，都可以采用数组形式的解构赋值

**2.2.1**

解构允许是定默认值

```javascript
let [foo = true] = [];
let [x, y = 'b'] = ['a']; // x = 'a'  y= 'b'
```

* 数组的元素是按次序排列的，变量的取值由它的位置决定；而对象的属性没有次序，变量必须与属性同名

```javascript
//如果变量名与属性名不一致,需要类似定义别名的方式
let { foo: baz } = { foo: 'aaa', bar: 'bbb' };	//foo:匹配模式 baz:变量
baz // "aaa"
```

**2.2.2 解构用途**

- 交换变量的值

```javascript
let x = 1;
let y = 2;
[x, y] = [y, x];
```

* 从函数返回多个值

```javascript
function example(){return [1, 2, 3];}
let [a,b, c] = example();
```

* 函数参数的定义

```javascript
function f([x,y,z]) { ... }
```

* 提取json数据

```javascript
let jsonData = {name:wintersweett,height:168,data[666,888]};
let {name,height,data} = jsonData;
```

- 遍历Map结构

```javascript
const map = new Map();
map.set('first', 'hello');
map.set('second', 'world');

for (let [key, value] of map) {
  console.log(key + " is " + value);
}
```

- 输入模块的指定方法

```javascript
const { SourceMapConsumer, SourceNode } = require("source-map");
```

**2.3 字符串**

2.3.1 字符串遍历

```javascript
for(let ch of 'zhang')
```

2.3.2 字符的Unicode表示

```javascript
'\z' === 'z'  // true
'\172' === 'z' // true
'\x7A' === 'z' // true
'\u007A' === 'z' // true
'\u{7A}' === 'z' // true
```

2.3.3 模板字符串

模板字符串（template string）是增强版的字符串，用反引号（`）标识。它可以当作普通字符串使用，也可以用来定义多行字符串，或者在字符串中嵌入变量。

**2.4 字符串新增方法**

* String.row

```javascript
let ret = String.raw`Hi\n${2+3}!`;	//Hi\n5!
```

* String.fromCodePoint

```javascript
String.fromCodePoint(0x78, 0x1f680, 0x79) === 'x\uD83D\uDE80y'	//true
String.fromCodePoint(0x20BB7)	//吉
```

* codePointAt

* includes,startWith,endsWith
* repeat(n)
* padStart, padEnd(ES2017新出)
* replece,replaceAll(2021)

```javascript
'aabbcc'.replace('b', '_')	// 'aa_bcc'
'aabbcc'.replace(/b/g, '_') // 'aa__cc'
// ********* 全都是返回新字符串,不会影响原字符串*********
```

**2.5 正则**

语法: `/正则表达式主体/修饰符(可选)`

`let patt = /zhang/i`

- **search() 方法** 用于检索字符串中指定的子字符串，或检索与正则表达式相匹配的子字符串，并返回子串的起始位置。
- **replace() 方法** 用于在字符串中用一些字符替换另一些字符，或替换一个与正则表达式匹配的子串。

```javascript
let regExp = new RegExp(partten, flag)
```

| i    | 执行对大小写不敏感的匹配。                               |
| ---- | -------------------------------------------------------- |
| g    | 执行全局匹配（查找所有匹配而非在找到第一个匹配后停止）。 |
| m    | 执行多行匹配。                                           |

**正则表达式模式**

| 表达式 | 描述                       |
| :----- | :------------------------- |
| [abc]  | 查找方括号之间的任何字符。 |
| [0-9]  | 查找任何从 0 至 9 的数字。 |
| (x\|y) | 查找任何以 \| 分隔的选项。 |

| 元字符 | 描述                                        |
| :----- | :------------------------------------------ |
| \d     | 查找数字。                                  |
| \s     | 查找空白字符。                              |
| \b     | 匹配单词边界。                              |
| \uxxxx | 查找以十六进制数 xxxx 规定的 Unicode 字符。 |

| 量词 | 描述                                  |
| :--- | :------------------------------------ |
| n+   | 匹配任何包含至少一个 *n* 的字符串。   |
| n*   | 匹配任何包含零个或多个 *n* 的字符串。 |
| n?   | 匹配任何包含零个或一个 *n* 的字符串。 |

* test()用于检测一个字符串是否匹配某个模式

```javascript
let patt =/e/;
patt.test('The best things in life are free!');
```

[RegExp详细规则](https://www.runoob.com/jsref/jsref-obj-regexp.html)

---

**2.6 数值拓展**

2.6.1 二进制和八进制表示

分别用前缀`0b`（或`0B`）和`0o`（或`0O`）表示

```javascript
0b101 === 3 	//true
0o767 === 503	//true octal
```

禁止转换

```javascript
let b = 0b1101;
let o = 0o1354;
let b1 = Number(b);
let o1 = Number(o);
let b2 = Number('0b1111');
```

2.6.2 Number.isFinite(),Number.isNaN();(全局也有 .isFinite isNaN方法)

NaN

NaN（Not a Number，非数）是计算机科学中数值数据类型的一类值，表示未定义或不可表示的值。常在浮点数运算中使用。首次引入NaN的是1985年的IEEE 754浮点数标准。

2.6.3 Number.isInteger

2.6.4 Number.isSafeInteger

`	Math.pow(2,50);` 返回2的50次方

ES6 引入了`Number.MAX_SAFE_INTEGER`和`Number.MIN_SAFE_INTEGER`这两个常量，用来表示这个范围的上下限。

**2.6.5  Math对象拓展**

* Math.trunc() `Math.trunc`方法用于去除一个数的小数部分，返回整数部分。

- Math.sign() 用来判断一个数到底是正数、负数、还是零。对于非数值，会先将其转换为数值。

2.6.6 指数运算符(es2016)

```javascript
2 ** 2 // 4
2 ** 3 // 8
```

**2.7 函数的拓展**

```javascript
function point(x = 0, y = 0){}
```

```javascript
// 写法一
function m1({x = 0, y = 0} = {}) {
  return [x, y];
}

// 写法二
function m2({x, y} = { x: 0, y: 0 }) {
  return [x, y];
}
// 函数没有参数的情况
m1() // [0, 0]
m2() // [0, 0]

// x 和 y 都有值的情况
m1({x: 3, y: 8}) // [3, 8]
m2({x: 3, y: 8}) // [3, 8]

// x 有值，y 无值的情况
m1({x: 3}) // [3, 0]
m2({x: 3}) // [3, undefined]

// x 和 y 都无值的情况
m1({}) // [0, 0];
m2({}) // [undefined, undefined]

m1({z: 3}) // [0, 0]
m2({z: 3}) // [undefined, undefined]
```

参数默认值的位置

通常情况下，定义了默认值的参数，应该是函数的尾参数。因为这样比较容易看出来，到底省略了哪些参数。如果非尾部的参数设置默认值，实际上这个参数是没法省略的。

```javascript
function f(x = 1, y){
    return [x,y];
}
f();	//[1,undefined]
f(2);	//[2,undefined]
f( , 1);//********错误
f(undefined, 1);	//[1,1]
```

**undefined作为函数参数可以触发默认值,null则没有办法触发默认值**

作用域

一旦设置了参数的默认值，函数进行声明初始化时，参数会形成一个单独的作用域（context）。等到初始化结束，这个作用域就会消失。这种语法行为，在不设置参数默认值时，是不会出现的。

```javascript
var x = 1;
function f(x, y = x) {
  console.log(y);
}
f(2) // 2
```

应用

利用参数默认值，可以指定某一个参数不得省略，如果省略就抛出一个错误

```javascript
function throwIfMissing() {
    throw new Error('Missing parameter');
}

function test(params = throwIfMissing()) {
    return params;
}
test();// Error: Missing parameter

function foo(optional = undefined) { ··· }	//如果默认值为undefined, 表示这个参数是可省略的
```

**2.7.1 name属性**

函数的name属性,返回该函数的函数名

```javascript
function test(){}
test.name //test
var f = function(){}
f.name // ""	 匿名函数
const bar = function xyz() {}
bar.name // xyz
```

**2.7.2 箭头函数**

```javascript
var f = v => v;
// ==
var f = function(v) {
    return v;
}
var f = () => 5;	//空参数就用一个括号即可
var f = (num1,num2) => num1 + num2;
let getObj = (id) => {id:id,name:'temp'}	//syntax error
let getObj = (id) => ({id:id,name:'temp'})	//ok
let fn = () => void doesNotReturn();	
//---------------------------箭头函数与解构函数使用---------------------------------
const full = ({first,last}) => first + " " + last;
//等同于
function full(person) {
    return person.first + " " + person.last;
}

let arr = [1,2,3].map(x => x * x);	//[1,4,9]
let sort = [5,4,12,6].sort((a,b) => a - b); //[4,5,6,12]

```

***箭头函数没有自己的this对象 (箭头函数导致`this`总是指向函数定义生效时所在的对象)

尾调用(Tail call)

某个函数的最后一步是调用另一个函数

```javascript
function f(x){
	return g(x);
}
```

**2.8 数组的拓展**

2.8.1 拓展运算符(spread)`...`

和rest类似互为逆运算,将一个数组转为用逗号分隔的参数序列

```javascript
function push(array, ...items){
    array.push(...items);
}
console.log(...[1,2,3])// 1 2 3
//------
function f(x,y,z){}
let args = [1,2,3];
f(...args);
//1. 数组合并
let arr1 = [0, 1, 2];
let arr2 = [3, 4, 5];
arr1.push(...arr2);
//2. 克隆数组
const a1 = [1, 2];
// 写法一
const a2 = [...a1];
// 写法二
const [...a2] = a1;
//3. 合并数组
const arr1 = ['a', 'b'];
const arr2 = ['c'];
[...arr1, ...arr2]	//浅拷贝
```

2.8.2 Array.from()

`Array.from`方法用于将两类对象转为真正的数组：类似数组的对象（array-like object）和可遍历（iterable）的对象.只要是部署了 Iterator 接口的数据结构，`Array.from`都能将其转为数组

**2.8.3 copyWithin()**

```javascript
Array.prototype.copyWithin(target, start = 0, end = this.length)
```

- target（必需）：从该位置开始替换数据。如果为负值，表示倒数。
- start（可选）：从该位置开始读取数据，默认为 0。如果为负值，表示从末尾开始计算。
- end（可选）：到该位置前停止读取数据，默认等于数组长度。如果为负值，表示从末尾开始计算。

```javascript
let arr = [0,1,2,3,4,5,6,7,8,9];
arr.copyWithin(5,0,5);
console.log(arr);	//0,1,2,3,4,0,1,2,3,4
```

2.8.4 数组市里的find() 和 findIndex();

2.8.5includes //判断是否包含给定的值

2.8.5flat(Infinity),flatMap()

flat: 展开多维数组,变成一位数组,不会改变原数组(默认只会展开一层,如果想要展开多层需要添加参数)

**2.8.6  Array.prototype.sort() 的排序稳定性**

常见的排序算法之中，插入排序、合并排序、冒泡排序等都是稳定的，堆排序、快速排序等是不稳定的。不稳定排序的主要缺点是，多重排序时可能会产生问题

**2.9 对象的拓展**

2.9.1 属性的简洁表示法

```javascript
let foo = "bar";
let key = 'value';
let baz = {foo,key};
console.log(JSON.stringify(baz));//{"foo":"bar","key":"value"}

//方法也可以简写
const o = {
  method() {
    return "Hello!";
  }
};
//等同于
const o = {
    method: function(){
        return "hello";
    }
}
```

2.9.2 属性表达式

```javascript
let obj = {};
obj.member = true;
obj["a" + "bc"] = 456;
console.log(JSON.stringify(obj))	// {"foo":true,"abc":456}
```

2.9.3 方法的name属性可以获取方法名(setter,getter除外)

2.9.4 对象属性的可枚举性和遍历(就是属性描述)

```javascript
let obj = { foo: 123 };
let prop = Object.getOwnPropertyDescriptor(obj, 'foo');
console.log(prop)	//{"value":456,"writable":true,"enumerable":true,"configurable":true}
```

**深拷贝一个对象**

```javascript
let obj = {name:'haha',age:18};
let clone = Object.assign(Object.create(Object.getPrototypeOf(obj)),obj);
```

2.9.5 链判断运算符

`?.`(链判断运算符ES2020引入)

```javascript
//安全写法
const firstName = message?.body?.user?.firstName || 'default';
iterator.return?.()	//判断对象方法是否存在，如果存在就立即执行
//iterator.return如果有定义，就会调用该方法，否则iterator.return直接返回undefined，不再执行?.后面的部分
```

2.9.10 Null判断运算符

读取对象属性的时候，如果某个属性的值是`null`或`undefined`，有时候需要为它们指定默认值。常见做法是通过`||`运算符指定默认值。

```javascript
const headerText = response.settings.headerText ?? 'Hello, world!';
const animationDuration = response.settings.animationDuration ?? 300;
const showSplashScreen = response.settings.showSplashScreen ?? true;
//结合eval操作符
const animationDuration = response.settings?.animationDuration ?? 300;
```

**2.10 对象的新增方法**

2.10.1 Object.is();

历史判断是否相等运算符: 相等运算符（`==`）和严格相等运算符（`===`）

`==`会自动转换数据类型进行比较 , `===`NaN +0 等于 -0,

```javascript
Object.is(+0, -0) // false
Object.is(NaN, NaN) // true
```

2.10.2 Object.assign()

`Object.assign()`方法用于对象的合并，将源对象（source）的所有可枚举属性，复制到目标对象（target）。

```javascript
const target = { a: 1 };
const source1 = { b: 2 };
const source2 = { c: 3 };
Object.assign(target, source1, source2);
target // {a:1, b:2, c:3}	
//--------------------多个对象属性同名,后者会覆盖前者
const target = { a: 1, b: 1 };
const source1 = { b: 2, c: 2 };
const source2 = { c: 3 };
Object.assign(target, source1, source2);
target // {a:1, b:2, c:3}
```

**注意点**

* 浅拷贝 Object.assign() 属于浅拷贝
* 同名属性替换
* 数组处理(会覆盖前面的数组)

**常见用途**

* 为对象添加属性

```javascript
let obj = {name:'haha',age:18};
Object.assign(obj,{sex:'ss'});
console.log(JSON.stringify(obj)); //{"name":"haha","age":18,"sex":"ss"}
//为对象添加方法
Object.assign(obj, {
            haha() {console.log("haha");},
            xixi(){console.log("xixi");}
        })
```

- 克隆对象

```javascript
let clone = Object.assign({},obj); //可以把obj 拷贝到新空对象
```

2.10.3 Object.keys(),Object.values(),Object.entres()

```javascript
let {keys, values, entries} = Object;	//得到对象的属性,实例
// entries成员是参数对象自身的（不含继承的）所有可遍历（enumerable）属性的键值对数组。
```

2.10.4 Object.fromEntries()

`Object.fromEntries()`方法是`Object.entries()`的逆操作，用于将一个键值对数组转为对象。

```javascript
Object.fromEntries([
  ['foo', 'bar'],
  ['baz', 42]
])
// { foo: "bar", baz: 42 }
```

**2.11 Symbol**

是一种基础数据类型,为了区分全局唯一

```javascript
let symbol = Symbol();	//不适new对象
let symbol2 = Symbol("description");	//可以添加描述
```

2.11.1Symbol.for(), Symbol.keyFor()

```javascript
let s1 = Symbol.for('foo');
let s2 = Symbol.for('foo');
s1 === s2 // true
```

Symbol.for 会检查全局缓存,如果有了直接返回,多次调用返回同一个值

Symbol() 会创建多次Symbol值

Symbol.keyFor() 返回已登记的Symbol类型值的key

```javascript
let s1 = Symbol.for("foo");
Symbol.keyFor(s1) // "foo"

let s2 = Symbol("foo");
Symbol.keyFor(s2) // undefined
```

*`Symbol.for()`为 Symbol 值登记的名字，是全局环境的，不管有没有在全局环境运行。

#### 三 Set和Map数据结构

```javascript
//简单使用
let ss = new Set();
[2,3,4,5,2,55,24,2,3,4].forEach(item =>ss.add(item));
for(let i of ss) {
   console.log(i);
}
//去重
let src = [1,1,2,1,2,3,2];
let temp = [...new Set(src)];
```

3.1.1set实例属性和方法

* size: 返回实例的成员总数
* add: 添加某个值,返回set本身
* delete: 删除某个值,返回是否删除成功或失败
* has(val): 返回bool,表示是否为set的成员

Array.from 方法可以将Set结构转换成数组

```javascript
const items = new Set([1, 2, 3, 4, 5]);
const array = Array.from(items);

// 例 数组去重
function dedupe(array){
    return Array.from(new Set(array));
}
dedupe([1,1,2,2,3,3]);	//[1,2,3]
```

3.1.2 遍历操作

- `Set.prototype.keys()`：返回键名的遍历器
- `Set.prototype.values()`：返回键值的遍历器
- `Set.prototype.entries()`：返回键值对的遍历器
- `Set.prototype.forEach()`：使用回调函数遍历每个成员

```javascript
// 遍历其他方式
let colors = new Set(['red','green','blue']);
for(let x of set);
colors.forEach((k,v) => console.log(`${key} : ${value}`));
// 解构操作
let set = new Set(['red', 'green', 'blue']);
let arr = [...set];
// ['red', 'green', 'blue']
```

3.2 WeakSet

与set的区别在于: 

1. WeakSet的成员只能是对象
2. WeakSet中的对象都是弱引用

只有三个方法

- **WeakSet.prototype.add(value)**：向 WeakSet 实例添加一个新成员。
- **WeakSet.prototype.delete(value)**：清除 WeakSet 实例的指定成员。
- **WeakSet.prototype.has(value)**：返回一个布尔值，表示某个值是否在 WeakSet 实例之中。

3.3 Map

* size: 返回成员总数
* Map.prototype.set(key,value)
* Map.prototype.get(key)
* Map.prototype.has(key)
* Map.prototype.delete(key)
* Map.prototype.clear()

map的遍历

```javascript
const map = new Map([
  ['F', 'no'],
  ['T',  'yes'],
]);
for (let key of map.keys()) {
  console.log(key);
}

for (let value of map.values()) {
  console.log(value);
}

for (let [key, value] of map.entries()) {
  console.log(key, value);
}

[...map.keys]	//[1,2,3]
[...map.values]	//[value, value2]
```

3.3.1 对象转Map

```javascript
// 1.
let obj = {"a":1, "b":2};
let map = new Map(Object.entries(obj));
```

3.3.2 Map -> JSON

```javascript
// 1. 键全是字符串
JSON.stringify(strMapToObj(strMap));
// 2.键有非字符串
JSON.stringify([...map]);
```

3.3.3 JSON -> Map

```
JSON.parse(jsonStr);
```

3.3.4 WeakMap

与Map区别

1. 只接收对象做键名
2. WeakMap键名所指向的对象,不计入垃圾回收机制;键名所引用对象都是弱引用,只要所引用的对象的其他引用都被清除，垃圾回收机制就会释放该对象所占用的内存。

3.3.5 WeakMap的语法

只有这四个方法可以用`get()`、`set()`、`has()`、`delete()`。

**3.3 Reflect**

**3.4 Proxy**

---

#### 四 Promise

Promise 是异步编程的一种解决方案

**4.1 promise 的含义**

简单说就是一个容器，里面保存着某个未来才会结束的事件（通常是一个异步操作）的结果

`Promise`特点:

* 对象状态不收外界影响
* 一单状态改变,就不会再改变,任何时候都可以得到这个结果

**4.2 基本用法**

```javascript
const promise = new Promise((resolve, reject) => {
  if (/* 异步操作成功 */){
    resolve(value);
  } else {
    reject(error);
  }
});
//获取结果
promise.then(res=>{}, fail=>{})

//promise可以嵌套
let p1 = new Promise((resolve, reject) => {
    resolve("p1 success");
})
let p2 = new Promise((resolve, reject) => {
    resolve("p2 success");
});
p1.then(res=>{
    console.log(res);	//p1 success
    return p2;
}).then(res => {
    console.log(res);	//p2 success
})
```

异常捕获(Promise.prototype.catch())

```javascript
let p1 = new Promise((resolve, reject) => {
    throw new Error("test error");
})
p1.catch(error=>{
	console.log(error);	//test error
})
//Promise 对象的错误具有“冒泡”性质，会一直向后传递，直到被捕获为止。
getJSON('/post/1.json').then(function(post) {
  return getJSON(post.commentURL);
}).then(function(comments) {
  // some code
}).catch(function(error) {
  // 处理前面三个Promise产生的错误
});
```

**一般总是建议，Promise 对象后面要跟`catch()`方法，这样可以处理 Promise 内部发生的错误。`catch()`方法返回的还是一个 Promise 对象，因此后面还可以接着调用`then()`方法。

```javascript
let testCatch = () => {
   return new Promise(function(resolve, reject) {
       resolve(x + 2); //x 没声明
   });
};
testCatch().catch(function(error) {
    console.log('oh no '+ error);
}).then(function() {
    console.log('carry on');
});
```

4.2.1 Promise.all(); 

​	const p = Promise.all([p1, p2, p3]);

- case1: 只有`p1`、`p2`、`p3`的状态都变成`fulfilled`，`p`的状态才会变成`fulfilled`，此时`p1`、`p2`、`p3`的返回值组成一个数组，传递给`p`的回调函数
- case2: 只要`p1`、`p2`、`p3`之中有一个被`rejected`，`p`的状态就变成`rejected`，此时第一个被`reject`的实例的返回值，会传递给`p`的回调函数。

```javascript
const p1 = new Promise((resolve,reject)=> {
    resolve("p111 success")
})
const p2 = new Promise((resolve,reject)=> {
    resolve("p222 success")
})
const p3 = new Promise((resolve,reject)=> {
    resolve("p333 success")
})
const all = Promise.all([p1,p2,p3]);
all.then(res=> {
     console.log(res);//p111 success,p222 success,p333 success
})
```

4.2.2 Promise.race(); 

const p = Promise.race([p1, p2, p3]);

任何一个实例率先改变状态,p的状态就跟着改变

4.2.3 Promise.allSettled(); 

只有等到所有这些参数实例都返回结果，不管是`fulfilled`还是`rejected`，包装实例才会结束。

4.2.3 Promise.any(); 

只要参数实例有一个变成`fulfilled`状态，包装实例就会变成`fulfilled`状态；如果所有参数实例都变成`rejected`状态，包装实例就会变成`rejected`状态。`Promise.any()`跟`Promise.race()`方法很像，只有一点不同，就是不会因为某个 Promise 变成`rejected`状态而结束。

**4.2.3 Promise.resolve() **

将现有对象转为 Promise 对象

```javascript
Promise.resolve('foo')
// 等价于
new Promise(resolve => resolve('foo'))
//如果参数是 Promise 实例，那么Promise.resolve将不做任何修改、原封不动地返回这个实例。
let thenable = {
  then: function(resolve, reject) {
    resolve(42);
  }
};
//参数不是具有then()方法的对象，或根本就不是对象
const p = Promise.resolve('Hello');
p.then(function (s) {
  console.log(s)
});
// 不带有任何参数Promise.resolve()方法允许调用时不带参数，直接返回一个resolved状态的 Promise 对象。
const p = Promise.resolve();

p.then(function () {
  // ...
});
```

4.2.4 Promise.reject()

```javascript
const p = Promise.reject('出错了');
// 等同于
const p = new Promise((resolve, reject) => reject('出错了'))
```

#### 五 Iterator 和 for of

**5.1 Iterator**

是一种接口，为各种不同的数据结构提供统一的访问机制。任何数据结构只要部署 Iterator 接口，就可以完成遍历操作（即依次处理该数据结构的所有成员）。

```javascript
// 例:自己实现迭代器
var it = makeIterator(['a', 'b']);

it.next() // { value: "a", done: false }
it.next() // { value: "b", done: false }
it.next() // { value: undefined, done: true }
function makeIterator(array) {
  var nextIndex = 0;
  return {
    next: function() {
      return nextIndex < array.length ?
        {value: array[nextIndex++], done: false} :
        {value: undefined, done: true};
    }
  };
}
```

```javascript
//数组的iterator属性
let str = "hello javascript";
let it = str[Symbol.iterator]();
let next = it.next();
while(!next.done) {
    console.log("value: " + next.value);
    next = it.next();
}
```

一个数据结构只要具有`Symbol.iterator`属性，就可以认为是“可遍历的”

使用场景:

* 解构赋值
* 拓展运算符`...`

**5.2`for...of`**

```javascript
const arr = ['red', 'green', 'blue'];
for(let v of arr) {
  console.log(v); // red green blue
}

const obj = {};
obj[Symbol.iterator] = arr[Symbol.iterator].bind(arr);

for(let v of obj) {
  console.log(v); // red green blue
}

//forEach            vs           for...of  
const arr = ['red', 'green', 'blue'];
arr.forEach(function (element, index) {
  console.log(element); // red green blue
  console.log(index);   // 0 1 2
});
//for...in循环读取键名，for...of循环读取键值
var arr = ['a', 'b', 'c', 'd'];
for (let a in arr) {
  console.log(a); // 0 1 2 3
}
for (let a of arr) {
  console.log(a); // a b c d
}
```

**Set 和 Map 结构**

```javascript
let map = new Map().set('a', 1).set('b', 2);
for (let pair of map) {
  console.log(pair);	//['a', 1]
}
for (let [key, value] of map) {
  console.log(key + ' : ' + value);
}

//Array.from  可以将一个长得象数组的结构转换成数组
let arrayLike = { length: 2, 0: 'a', 1: 'b' };
// 正确
for (let x of Array.from(arrayLike)) {
    console.log(x);     // a  b
}
```

**数组的其他遍历方式**

```javascript
let myArray = ['hello','moto'];
for (var index = 0; index < myArray.length; index++) {
  console.log(myArray[index]);
}
myArray.forEach(function (value) {	//缺点是没有办法跳出循环
  console.log(value);
});
```

`for...in`循环有几个缺点:

1. 数组的键名是数字，但是`for...in`循环是以字符串作为键名“0”、“1”、“2”等等。
2. `for...in`循环不仅遍历数字键名，还会遍历手动添加的其他键，甚至包括原型链上的键。
3. 某些情况下，`for...in`循环会以任意顺序遍历键名。

**推荐使用`for...of`**

- 有着同`for...in`一样的简洁语法，但是没有`for...in`那些缺点。
- 不同于`forEach`方法，它可以与`break`、`continue`和`return`配合使用。
- 提供了遍历所有数据结构的统一操作接口。

#### 六 Generator

首先可以把它理解成，Generator 函数是一个状态机，封装了多个内部状态。

```javascript
function* helloGenerator(){
    yield 'hello';
    yield 'world';
    return 'result';
}
//use
let haha = helloGenerator();
console.log(JSON.stringify(haha.next()));//{"value":"hello","done":false}
console.log(JSON.stringify(haha.next()));//{"value":"world","done":false}
```



```javascript
function* longRunningTask(value1) {
  try {
    var value2 = yield step1(value1);
    var value3 = yield step2(value2);
    var value4 = yield step3(value3);
    var value5 = yield step4(value4);
    // Do something with value4
  } catch (e) {
    // Handle any error from step1 through step4
  }
}
```

#### 七 async

#### 八 Class

**8.1 类的基本语法**

```javascript
//一个简单的类例子
function Point(x, y) {
  this.x = x;
  this.y = y;
}

Point.prototype.toString = function () {
  return '(' + this.x + ', ' + this.y + ')';
};
var p = new Point(1, 2);

const MyClass = class { /* ... */ };		//ok

let person = new class {					//ok
  constructor(name) {
    this.name = name;
  }
  sayName() {
    console.log(this.name);
  }
}('张三');
person.sayName(); // "张三"	
```

**类的静态方法**

```javascript
class Foo {
  static classMethod() {
    return 'hello';
  }
}

Foo.classMethod() // 'hello'

var foo = new Foo();
foo.classMethod()	//TypeError: foo.classMethod is not a function
```

**8.2 类的继承**

```javascript
class Point {
}

class ColorPoint extends Point {
  constructor(x, y, color) {
    super(x, y); // 调用父类的constructor(x, y)
    this.color = color;
  }

  toString() {
    return this.color + ' ' + super.toString(); // 调用父类的toString()
  }
}

//错误用法
class ColorPoint extends Point {
  constructor(x, y, color) {
    this.color = color; // ReferenceError
    super(x, y);
    this.color = color; // 正确
  }
}
```

`***在子类的构造函数中，只有调用`super`之后，才可以使用`this`关键字，否则会报错。这是因为子类实例的构建，基于父类实例，只有`super`方法才能调用父类实例`。

#### 九 Module

**9.1 export命令**

模块功能主要由两个命令构成：`export`和`import`。`export`命令用于规定模块的对外接口，`import`命令用于输入其他模块提供的功能。

```javascript
//export 两种方式 1 
export var firstName = 'Michael';
export var lastName = 'Jackson';
export var year = 1958;
// 两种方式 2 
var firstName = 'Michael';
var lastName = 'Jackson';
var year = 1958;

export { firstName, lastName, year };
```

`export`导出函数

```javascript
export function multiply(x, y) {
  return x * y;
};
```

`export`导出时改变导出的名称

```javascript
function v1() {...}
export{
	v1 as aliasV1;
}

// 写法一
export var m = 1;

// 写法二
var m = 1;
export {m};

// 写法三
var n = 1;
export {n as m};
```

**9.2 import 命令**

```javascript
import { firstName, lastName, year } from '../../common/utils.js';
```

---



#### 十 编程风格

**变量**

- 能用let就不用var
- 能用const 就不用let(编译器会对const进行优化)

**字符串**

静态字符串一律使用单引号或反引号，不使用双引号。动态字符串使用反引号。

```javascript
// bad
const a = "foobar";
const b = 'foo' + a + 'bar';

// acceptable
const c = `foobar`;

// good
const a = 'foobar';
const b = `foo${a}bar`;
```

**解构赋值**

使用数组成员对变量赋值时，优先使用解构赋值。

```javascript
const arr = [1, 2, 3, 4];
// bad
const first = arr[0];
const second = arr[1];
// good
const [first, second] = arr;
```



函数的参数如果是对象的成员，优先使用解构赋值。

```javascript
// bad
function getFullName(user) {
  const firstName = user.firstName;
  const lastName = user.lastName;
}
// good
function getFullName(obj) {
  const { firstName, lastName } = obj;
}
// best
function getFullName({ firstName, lastName }) {
}
```

**对象**

单行定义的对象，最后一个成员不以逗号结尾。多行定义的对象，最后一个成员以逗号结尾

```javascript
// bad
const a = { k1: v1, k2: v2, };
const b = {
  k1: v1,
  k2: v2
};
// good
const a = { k1: v1, k2: v2 };
const b = {
  k1: v1,
  k2: v2,
};
```

对象尽量静态化，一旦定义，就不得随意添加新的属性。如果添加属性不可避免，要使用`Object.assign`方法。

```javascript
// bad
const a = {};
a.x = 3;

// if reshape unavoidable
const a = {};
Object.assign(a, { x: 3 });

// good
const a = { x: null };
a.x = 3;
```

对象的属性和方法，尽量采用简洁表达法，这样易于描述和书写。

```javascript
var ref = 'some value';
// bad
const atom = {
  ref: ref,
  value: 1,
  addValue: function (value) {
    return atom.value + value;
  },
};
// good
const atom = {
  ref,
  value: 1,
  addValue(value) {
    return atom.value + value;
  },
};
```

**数组**

使用扩展运算符（...）拷贝数组。

```javascript
// bad
const len = items.length;
const itemsCopy = [];
let i;
for (i = 0; i < len; i++) {
  itemsCopy[i] = items[i];
}
// good
const itemsCopy = [...items];
```

使用 Array.from 方法，将类似数组的对象转为数组。

```javascript
const foo = document.querySelectorAll('.foo');
const nodes = Array.from(foo);
```

**函数**

立即执行函数可以写成箭头函数的形式。

```javascript
(() => {
  console.log('Welcome to the Internet.');
})();
```

那些使用匿名函数当作参数的场合，尽量用箭头函数代替。因为这样更简洁，而且绑定了 this。

```javascript
// bad
[1, 2, 3].map(function (x) {
  return x * x;
});
// good
[1, 2, 3].map((x) => {
  return x * x;
});
// best
[1, 2, 3].map(x => x * x);
```

箭头函数取代`Function.prototype.bind`，不应再用 self/_this/that 绑定 this

```javascript
// bad
const self = this;
const boundMethod = function(...params) {
  return method.apply(self, params);
}
// acceptable
const boundMethod = method.bind(this);
// best
const boundMethod = (...params) => method.apply(this, params);
```

所有配置项都应该集中在一个对象，放在最后一个参数，布尔值不可以直接作为参数。

```javascript
// bad
function divide(a, b, option = false ) {
}
// good
function divide(a, b, { option = false } = {}) {
}
```

使用默认值语法设置函数参数的默认值。

```javascript
// bad
function handleThings(opts) {
  opts = opts || {};
}
// good
function handleThings(opts = {}) {
  // ...
}
```

**Map结构**

注意区分 Object 和 Map，只有模拟现实世界的实体对象时，才使用 Object。如果只是需要`key: value`的数据结构，使用 Map 结构。因为 Map 有内建的遍历机制。

```javascript
let map = new Map(arr);
for (let key of map.keys()) {
  console.log(key);
}
for (let value of map.values()) {
  console.log(value);
}
for (let item of map.entries()) {
  console.log(item[0], item[1]);
}
```

**Class**

总是用 Class，取代需要 prototype 的操作。因为 Class 的写法更简洁，更易于理解

```javascript
// bad
function Queue(contents = []) {
  this._queue = [...contents];
}
Queue.prototype.pop = function() {
  const value = this._queue[0];
  this._queue.splice(0, 1);
  return value;
}
// good
class Queue {
  constructor(contents = []) {
    this._queue = [...contents];
  }
  pop() {
    const value = this._queue[0];
    this._queue.splice(0, 1);
    return value;
  }
}
```

使用`extends`实现继承，因为这样更简单，不会有破坏`instanceof`运算的危险。

```javascript
// bad
const inherits = require('inherits');
function PeekableQueue(contents) {
  Queue.apply(this, contents);
}
inherits(PeekableQueue, Queue);
PeekableQueue.prototype.peek = function() {
  return this._queue[0];
}

// good
class PeekableQueue extends Queue {
  peek() {
    return this._queue[0];
  }
}
```

**模块**

Module 语法是 JavaScript 模块的标准写法，坚持使用这种写法。使用`import`取代`require`。

````javascript
// bad
const moduleA = require('moduleA');
const func1 = moduleA.func1;
const func2 = moduleA.func2;
// good
import { func1, func2 } from 'moduleA';
````

使用`export`取代`module.exports`。

```javascript
// commonJS的写法
var React = require('react');
var Breadcrumbs = React.createClass({
  render() {
    return <nav />;
  }
});
module.exports = Breadcrumbs;

// ES6的写法
import React from 'react';

class Breadcrumbs extends React.Component {
  render() {
    return <nav />;
  }
};
export default Breadcrumbs;
```

**如果模块只有一个输出值，就使用`export default`，如果模块有多个输出值，就不使用`export default`，`export default`与普通的`export`不要同时使用。

不要在模块输入中使用通配符。因为这样可以确保你的模块之中，有一个默认输出（export default）。

```javascript
// bad
import * as myObject from './importModule';

// good
import myObject from './importModule';
```

如果模块默认输出一个函数，函数名的首字母应该小写。

如果模块默认输出一个对象，对象名的首字母应该大写。

```javascript
function makeStyleGuide() {
}
export default makeStyleGuide;
```

```javascript
const StyleGuide = {
  es6: {
  }
};
export default StyleGuide;
```

