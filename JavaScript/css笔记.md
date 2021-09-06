**CSS背景**

- background-color
- background-image

```
background-image: url("paper.gif");
```

- background-repeat:	水平和垂直方向上都重复图像
- background-attachment
- background-position
- background-size

属性简写

```css
background: #ffffff url("tree.png") no-repeat right top;
```

**CSS高度和宽度**

**注意：**请记住，**height** 和 **width** 属性不包括内边距、边框或外边距！它们设置的是元素的内边距、边框和外边距内的区域的高度/宽度！。(比如一个div padding=50px,w h 各100px; 那么看起来这个div就有150px大)

- initial - 将高度/宽度设置为默认值。
- inherit - 从其父值继承高度/宽度。

**注释：**`max-width` 属性的值将覆盖 `width`（宽度）。

**CSS文本对齐**

text-align 属性用于设置文本的水平对齐方式(左对齐或右对齐，或居中对齐)。

- justify:	将拉伸每一行，以使每一行具有相等的宽度，并且左右边距是直的

vertical-align 属性设置元素的垂直对齐方式。

**CSS文本装饰**

text-decoration 属性用于设置或删除文本装饰。

text-decoration: none; 通常用于从链接上删除下划线：

- overline
- line-through
- underline

**CSS文本转换**

text-transform 属性用于指定文本中的大写和小写字母。

**CSS文字间距**

- 文字缩进:	text-indent 属性用于指定文本第一行的缩进：
- 字母间距:    letter-spacing 属性用于指定文本中字符之间的间距。
- 行高:            line-height 属性用于指定行之间的间距
- 字间距(单词):    word-spacing 属性用于指定文本中单词之间的间距。
- 空白:                  white-space 属性指定元素内部空白的处理方式。

**CSS文本阴影**

text-shadow 属性为文本添加阴影。

**CSS字体样式**

font-style 属性主要用于指定斜体文本。

- normal - 文字正常显示
- italic - 文本以斜体显示
- oblique - 文本为“倾斜”（倾斜与斜体非常相似，但支持较少）

**CSS字体大小**

为了允许用户调整文本大小（在浏览器菜单中），许多开发人员使用 em 而不是像素。

W3C 建议使用 em 尺寸单位。

1em 等于当前字体大小。浏览器中的默认文本大小为 16px。因此，默认大小 1em 为 16px。

可以使用 vw 单位设置文本大小，它的意思是“视口宽度”（"viewport width"）。

视口（Viewport）是浏览器窗口的大小。 1vw = 视口宽度的 1％。如果视口为 50 厘米宽，则 1vw 为 0.5 厘米。

**CSS布局 - display属性**

块级元素(div,h1-h6,p,form,header,footer,section)

行内元素(span,a,img)

- block: 块级元素总是从新行开始，并占据可用的全部宽度（尽可能向左和向右伸展）。
- inline: 内联元素不从新行开始，仅占用所需的宽度。
- none: 隐藏

**position属性**

- static: 元素默认情况下的定位方式为 static（静态）。静态定位的元素不受 top、bottom、left 和 right 属性的影响。
- relative: 元素相对于正常位置定位,top,right,bottom,left属性将导致其偏离其正常位置进行调整。不会对其余内容进行调整来适应元素留下的任何空间(整体移动,宽高不变)。
- fixed: 相对于视口(窗口)定位,不会随着页面滚动而滚动.trbl确定位置
- absolute: 元素相对于最近的定位祖先元素进行定位,如果绝对定位的元素没有祖先，它将使用文档主体（body），并随页面滚动一起移动。
- sticky: 粘性元素根据滚动位置在相对（relative）和固定（fixed）之间切换.现象:当页面滑动时会跟着页面一起滑动,当滑动到顶部时,就固定在哪里不再滑动.

---

**CSS 布局 - 溢出**

- overflow: 属性指定在元素的内容太大而无法放入指定区域时是剪裁内容还是添加滚动条。
  - visible - 默认。溢出没有被剪裁。内容在元素框外渲染
  - hidden - 溢出被剪裁，其余内容将不可见
  - scroll - 溢出被剪裁，同时添加滚动条以查看其余内容
  - auto - 与 scroll 类似，但仅在必要时添加滚动条

**CSS 浮动和清除**

- float 属性规定元素如何浮动。

- clear 属性规定哪些元素可以在清除的元素旁边以及在哪一侧浮动。

**display属性**

- inline:	虽然设置宽高,但是无效,只会是实际内容大小.
- inline-block:       相对于inline就是宽高设置有效了
- block:      每一个块都新起一行

**CSS水平和垂直对齐**

margin

* auto:     自身相对于父容器水平居中

**居中对齐图象**

需要将左右外边距设置为`auto`,并将其设置为块元素

```css
img {
  display: block;
  margin-left: auto;
  margin-right: auto;
  width: 40%;
}
```

**对齐元素方法**

*  `position: absolute; ` + `right/left`
* `float: left/right`

**注意：**如果一个元素比包含它的元素高，并且它是浮动的，它将溢出其容器。您可以使用 *clearfix hack* 来解决此问题（请看下面的例子）

垂直对齐

```css
.center{
	line-height: 200px;	//或者设置上下padding
	text-align: center;
}
```

**CSS组合选择器**

- 后代选择器 (空格):	后代选择器匹配作为指定元素后代的所有元素。

```css
div > p {
 background-color: yellow;
}
<div>
  <p>div 中的段落 1。</p><!-- 会有黄色背景 -->
  <section><p>div 中的段落 3。</p></section><!-- 会有黄色背景 -->
</div>
```

- 子选择器 (>):	选择属于指定元素子元素的所有元素。

```css
div > p {
 background-color: yellow;
}
<div>
 <p>div 中的段落 1。</p>
 <section><p>div 中的段落 3。</p></section> <!-- 非子但属后代(不会有黄色背景) -->
</div>
```

- 相邻兄弟选择器 (+):	选择所有作为指定元素的相邻的同级元素

```css
div + p {
  background-color: yellow;
}
<div>
  <p>div 中的段落 1。</p>	<!-- 没有黄色背景 -->
  <p>div 中的段落 2。</p>	<!-- 没有黄色背景 -->
</div>
<p>段落 3。不在 div 中。</p> 	<!-- 有黄色背景 -->
```

- 通用兄弟选择器 (~):	指定元素的所有同级元素

```css
div ~ p {
  background-color: yellow;
}
<div>
  <p>段落 2。</p> 		<!-- 没有黄色背景 -->
</div>
<p>段落 3。</p>	<!-- 有黄色背景 -->
<code>一些代码。</code>	<!-- 没有黄色背景 -->
<p>段落 4。</p>	<!-- 有黄色背景 -->

```

| 选择器                                                       | 示例    | 示例描述                                   |
| :----------------------------------------------------------- | :------ | :----------------------------------------- |
| [*element* *element*](https://www.w3school.com.cn/cssref/selector_element_element.asp) | div p   | 选择 <div> 元素内的所有 <p> 元素。         |
| [*element*>*element*](https://www.w3school.com.cn/cssref/selector_element_gt.asp) | div > p | 选择其父元素是 <div> 元素的所有 <p> 元素。 |
| [*element*+*element*](https://www.w3school.com.cn/cssref/selector_element_plus.asp) | div + p | 选择所有紧随 <div> 元素之后的 <p> 元素。   |
| [*element1*~*element2*](https://www.w3school.com.cn/cssref/selector_gen_sibling.asp) | p ~ ul  | 选择前面有 <p> 元素的每个 <ul> 元素。      |

**CSS属性选择器**

```css
a[target] {
  background-color: yellow;
}
<a href="https://www.w3school.com">w3school.com.cn</a>	<!-- 没有背景 -->
<a href="http://www.disney.com" target="_blank">disney.com</a> <!-- 黄色背景 -->
```

**CSS圆角**

四个值 - `border-radius: 15px 50px 30px 5px;`（依次分别用于：左上角、右上角、右下角、左下角）

**CSS渐变**

```css
//线性渐变
#grad {
  background-image: linear-gradient(to right, red,orange,yellow,green,blue,indigo,violet); 
}
//径向渐变
#grad {
  background-image: radial-gradient(circle, red, yellow, green);//circle是形状
}
```

**CSS阴影**

* 文字阴影: 水平阴影,垂直阴影, 模糊效果 阴影颜色

```css
h1 {
  text-shadow: 2px 2px 5px red;
}
```

- 卡片阴影

```css
div.card {
  width: 250px;
  box-shadow: 0 4px 8px 0 rgba(0, 0, 0, 0.2), 0 6px 20px 0 rgba(0, 0, 0, 0.19);
  text-align: center;
}
```

---

#### 文本效果

CSS `text-overflow` 属性规定应如何向用户呈现未显示的溢出内容。

**单行文字省略号**

```css
white-space: nowrap;  //不要换行
overflow: hidden;		//超出部分隐藏
text-overflow: clip;	//clip  ellipsis 裁切文字 省略号
word-wrap: break-word;	//允许长单词被打断并换行到下一行
```

---

**CSSvar()函数语法**

```css
:root {
  --blue: #1e90ff;
  --white: #ffffff; 
}
.use {
  color: var(--blue);
  background-color: var(--white);
}
```

**CSS Box Sizing**

CSS  属性允许我们在元素的总宽度和高度中包括内边距（填充）和边框。

默认情况下，元素的宽度和高度是这样计算的：

- width + padding + border = 元素的实际宽度
- height + padding + border = 元素的实际高度

`box-sizing` 定义元素宽度和高度的计算方式：它们是否应包含内边距（padding）和边框。

```
* {
  box-sizing: border-box;
}
```

**柔性盒子 FlexBox**

`display: flex;`

- **flex-direction**:     属性定义容器要在哪个方向上堆叠 flex 项目
  - column:          设置垂直堆叠 flex 项目（从上到下）
  - column-reverse:  垂直堆叠 flex 项目（但从下到上）
  - row:         水平堆叠 flex 项目（从左到右）
  - row-reverse:        水平堆叠 flex 项目（但从右到左）
- **flex-wrap**:       属性规定是否应该对 flex 项目换行
  - wrap          值规定 flex 项目将在必要时进行换行
  - nowrap           值规定将不对 flex 项目换行（默认）
  - wrap-reverse           值规定如有必要，弹性项目将以相反的顺序换行
- **flex-flow**:         用于同时设置 flex-direction 和 flex-wrap 属性的简写属性
- **justify-content**:        用于对齐 flex 项目
  - center:      将 flex 项目在容器的中心对齐
  - flex-start:      flex 项目在容器的开头对齐（默认）
  - flex-end:       将 flex 项目在容器的末端对齐
  - space-around:     显示行之前、之间和之后带有空格的 flex 项目(将间距均分)
  - space-between:     从两边均匀分布
- **align-items**:  用于垂直对齐flex项目
  - center:        居中对齐
  - flex-start:   容器顶部对齐
  - flex-end:     容器底部对齐
  - stretch 值拉伸 flex 项目以填充容器（默认）
  - baseline:  使flex项目基线对齐
- **align-content** :   用于对齐弹性线
  - space-between 显示的弹性线之间有相等的间距
  - space-around  显示弹性线在其之前、之间和之后带有空格
  - stretch 值拉伸弹性线以占据剩余空间（默认）
  - center 值在容器中间显示弹性线
  - flex-start 值在容器开头显示弹性线：
  - flex-end 值在容器的末尾显示弹性线：

完美居中:

```css
.flex-container {
  display: flex;
  height: 300px;
  justify-content: center;
  align-items: center;
}
```

**子元素(项目)**

***flex 容器的直接子元素会自动成为弹性（flex）项目***。

用于弹性项目的属性有：

- order		规定 flex 项目的顺序
- flex-grow           规定某个 flex 项目相对于其余 flex 项目将增长多少
- flex-shrink         规定某个 flex 项目相对于其余 flex 项目将收缩多少
- flex-basis           规定 flex 项目的初始长度
- flex                     是 flex-grow、flex-shrink 和 flex-basis 属性的简写属性。
- align-self

**align-self属性**

`align-self `属性规定弹性容器内所选项目的对齐方式。

`align-self` 属性将覆盖容器的 `align-items `属性所设置的默认对齐方式。

