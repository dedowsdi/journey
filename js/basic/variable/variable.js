'use strict'
// let only works in strict mode
var x = "global"; // define x property in global object
let y = "global"; //
console.log(this.x); // global in browser, undefined in nodejs?
console.log(this.y); // undefined
function scopeTest(){
    var x = 1;
    let y = 1;
    {
        var x = 2; // save variable
        let y = 2; // new variable in current block
        console.log(x);
        console.log(y);
    }
    console.log(x);
    console.log(y);
}

scopeTest();
