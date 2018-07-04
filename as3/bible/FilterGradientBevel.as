package
{
    import flash.display.Sprite;

    public class FilterGradientBevel extends Sprite
    {
        public function FilterGradientBevel()
        {
            var b:BeveledButton;
            b = new BeveledButton("Click me");
            b.x = b.y = 100;
            addChild(b);
            b = new BeveledButton("Please don’t click me");
            b.x = 200; b.y = 100;
            addChild(b);
        }
    }
}

import flash.display.*;
import flash.text.*;
import flash.filters.GradientBevelFilter;
import flash.events.MouseEvent;

class BeveledButton extends Sprite {

    protected var _label:String;
    protected var tf:TextField;
    protected var bg:Shape;
    protected const ANGLE:Number = -10;
    protected const MARGIN_W:Number = 15, MARGIN_H:Number = 8;

    public function BeveledButton(initialLabel:String = "") {
        bg = new Shape();
        addChild(bg);

        tf = new TextField();
        tf.defaultTextFormat = new TextFormat("_sans", 11, 0xffffff, true);
        tf.selectable = false;
        tf.multiline = false;
        tf.autoSize = TextFieldAutoSize.LEFT;
        addChild(tf);

        mouseChildren = false;
        buttonMode = true;

        var colors:Array = [0x606060, 0x909090, 0xe0e0e0];
        var alphas:Array = [100, 100, 100];
        var ratios:Array = [0, 127, 255];

        bg.filters=[new GradientBevelFilter(3, 90+ANGLE, colors, alphas, ratios)];
        label = initialLabel;
        addEventListener(MouseEvent.MOUSE_DOWN, onMouseDown);
        addEventListener(MouseEvent.MOUSE_UP, onMouseUp);
    }

    public function set label(s:String):void {
        tf.width = tf.height = 0;
        tf.text = s;
        var W:Number = tf.textWidth + 2*MARGIN_W;
        var H:Number = tf.textHeight - 4 + 2*MARGIN_H;
        tf.y = MARGIN_H - 4;
        tf.x = MARGIN_W;
        bg.graphics.clear();
        bg.graphics.beginFill(0);
        bg.graphics.drawRoundRect(0, 0, W, H, 8);
        bg.graphics.endFill();
    }

    public function get label():String {return _label;}

    protected function onMouseDown(event:MouseEvent):void {
        var bevel:GradientBevelFilter = GradientBevelFilter(bg.filters[0]);
        bevel.angle = -90 + ANGLE;
        bg.filters = [bevel];
        tf.x += 1;
        tf.y += 1;
    }

    protected function onMouseUp(event:MouseEvent):void {
        var bevel:GradientBevelFilter = GradientBevelFilter(bg.filters[0]);
        bevel.angle = 90 + ANGLE;
        bg.filters = [bevel];
        tf.x -= 1;
        tf.y -= 1;
    }
}