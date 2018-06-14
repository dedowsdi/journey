package
{
    import flash.display.Sprite;
    import flash.display.SimpleButton;
    import flash.geom.Point;
    import flash.events.MouseEvent;
    import flash.display.DisplayObjectContainer;
    import flash.display.DisplayObject;
    import com.tuto.SimpleTextButton;
    import flash.events.KeyboardEvent;
    import flash.events.Event;
    import flash.ui.Keyboard;
    import flash.text.TextField;
    import flash.text.TextFieldType;
    import flash.text.TextFormat;

    // hold key to move brick every frame
    public class Focus extends Sprite{

        protected var input:TextField;
        protected var submitButton:TestButton;
        protected var clearButton:TestButton;

        public function Focus(){
            input = new TextField();
            input.type = TextFieldType.INPUT;
            input.defaultTextFormat = new TextFormat("_sans", 12, 0);
            input.width = 215;
            input.height = 20;
            input.border = true;
            addChild(input);
            input.x = 10;
            input.y = 10;
            input.addEventListener(KeyboardEvent.KEY_UP, onInputKey);

            submitButton = new TestButton(100, 30, "Submit");
            addChild(submitButton);
            submitButton.x = 10;
            submitButton.y = 40;
            submitButton.addEventListener(MouseEvent.CLICK, onSubmit);

            clearButton = new TestButton(100, 30, "Clear");
            addChild(clearButton);
            clearButton.x = 125;
            clearButton.y = 40;
            clearButton.addEventListener(MouseEvent.CLICK, onClear);

            // init focus to input
            stage.focus = input;
        }

        protected function onInputKey(evt:KeyboardEvent):void
        {
            if(evt.keyCode == Keyboard.ENTER){
                onSubmit(null);
            }
        }

        protected function onSubmit(evt:MouseEvent):void
        {
            stage.focus = null;
            submitButton.label.text = "Submitted!";
            // disable input abilitity if it's submitted
            input.mouseEnabled = input.tabEnabled = false;
            input.background = true;
            input.backgroundColor = 0xb0b0b0;
            clearButton.removeEventListener(MouseEvent.CLICK, onClear);
        }

        protected function onClear(evt:MouseEvent):void
        {
            input.text = "";
            stage.focus = input;
        }

    }
}

import flash.display.*;
import flash.text.*;
class TestButton extends Sprite {
    public var label:TextField;
    public function TestButton(w:Number, h:Number, labelText:String) {
        graphics.lineStyle(0.5, 0, 0, true);
        graphics.beginFill(0xa0a0a0);
        graphics.drawRoundRect(0, 0, w, h, 8);

        label = new TextField();
        addChild(label);
        label.defaultTextFormat = new TextFormat("_sans", 11, 0, true, false,
        false, null, null, "center");
        label.width = w;
        label.height = h;
        label.text = labelText;
        label.y = (h - label.textHeight)/2 - 2;
        buttonMode = true;
        mouseChildren = false;
    }
}