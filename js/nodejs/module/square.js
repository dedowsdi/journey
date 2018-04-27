'use strict'
// muse use module.exports, not exports
module.exports = class Square{
    constructor(width){
        this.width = width;
    }

    area(){
        return this.width * this.width;
    }
};
