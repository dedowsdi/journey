package com.tuto{

import flash.display.Sprite;
import flash.display.SimpleButton;
import mx.skins.spark.ButtonSkin;
import flash.geom.Point;
import flashx.textLayout.formats.WhiteSpaceCollapse;

    public class SimpleTextButton extends SimpleButton{
        public var text:String;
        public var fontSize:uint;

        public function SimpleTextButton(text:String, fontSize:uint = 30){
            this.text = text;
            this.fontSize = fontSize;
            init();
        }

        public function init():void{
            this.upState = new ButtonState(0x00ff00, text, fontSize);
            this.downState = new ButtonState(0xff0000, text, fontSize);
            this.overState = new ButtonState(0x0000ff, text, fontSize);
            this.hitTestState = this.overState;
            this.useHandCursor = true;
        }
    }

}

import flash.text.TextField;
import flash.text.TextFormat;
import flash.text.TextFieldAutoSize;
import flash.text.TextFormatAlign;

class ButtonState extends TextField{
    private var rectColor:uint;
    private var fontSize:uint;
    
    public function ButtonState(color:uint, text:String, fontSize:uint){

        this.background = true;
        this.backgroundColor = color;

        var format:TextFormat = new TextFormat;
        format.size = fontSize;
        format.bold = true;
        format.color = 0x000000;
        // format.align = TextFormatAlign.CENTER;  // what's this used for? only useful if auto size is false?

        this.defaultTextFormat = format;
        this.text = text;
        // it's tricky to set auto size. imagin this text is rended in default size(100), then it get auto sized, use rule specified by you.
        this.autoSize = TextFieldAutoSize.LEFT;
    }

}