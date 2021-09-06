### promise例子(入门那会儿看的)

```javascript
let test = (params) => new Promise((resolve,reject) => {
  console.log("test",params);
  resolve("成功了");
});

function cook() {
  console.log('api# cook 开始做饭。');
  var p = new Promise(function(resolve, reject){        //做一些异步操作
      setTimeout(function(){
          console.log('api# cook 做饭完毕！准备返回');
          resolve('api# cook 鸡蛋炒饭');
      }, 1000);
  });
  return p;
}

function eat(params) {
  console.log("api # eat  吃饭",params);
  var p = new Promise(function(resolve, reject){        //做一些异步操作
      setTimeout(function(){
          console.log('api # eat 吃饭完毕');
          resolve('api # eat 返回 需要洗碗 ,一双筷子 一个碗');
      }, 1000);
  });
  return p;
}

function wash(params) {
  console.log("api# wash 洗碗 参数->",params);
  var p = new Promise((resolve,reject) =>{
    console.log("wash",params);
    setTimeout(()=>{
      console.log("api# wash  洗完碗了");
      resolve("api# wash 返回 洗完碗了");
    })
    return p;
  })
}

function normal(params) {
  let normal = new Promise(function haha(resolve, reject) {
    console.log("normal==>",params);
    resolve("dsfsdfsd");
  })
  return normal;
}

let lambda = () => new Promise((resolve,reject)=>{
  console.log("lambda");
  resolve("lambda 返回值");
});

let lambda2 = () => new Promise((resolve,reject)=>{
  console.log("lambda2-------");
  resolve("lambda2 返回值");
});

module.exports = {
  lambda2,
  lambda,
  cook,
  eat,
  wash,
  test,
  normal
}
```

