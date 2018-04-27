var o = {p: 1, q:true};
var {p, q} = o;
console.log(p);
console.log(q);

var {p:2, q:false} = o; // assignment
console.log(p);
console.log(q);

var a = [1, 2, 3];
var [a0, a1, a2] = a;
console.log(a0);
console.log(a1);
console.log(a2);
