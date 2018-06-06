package
{
    import flash.display.Sprite;
    import flash.display.SimpleButton;
    import flash.geom.Point;
    import flash.events.MouseEvent;
    import flash.display.DisplayObjectContainer;
    import flash.display.DisplayObject;
    import com.tuto.SimpleTextButton;
    import org.osmf.layout.AbsoluteLayoutMetadata;

    // weird thing happen if you turn off mouseChildren of root object, the root object itself will stop accept mouse event.
    public class BlockMouse extends Sprite{
        protected var screen:Sprite;
        public function BlockMouse(){
            makeTestButton();
        }

        protected function makeTestButton():void
        {
            var button:TestButton;
            button = new TestButton(100, 30, "Pizza");
            addChild(button);
            button.x = 10;
            button.y = 10;
            button.addEventListener(MouseEvent.CLICK, onPizzaClick);

            button = new TestButton(100, 30, "Tacos");
            addChild(button);
            button.x = 130;
            button.y = 10;
            button.addEventListener(MouseEvent.CLICK, onTacosClick);

            button = new TestButton(110, 30, "SCREEN ON!");
            addChild(button);
            button.x = 250;
            button.y = 10;
            button.addEventListener(MouseEvent.CLICK, onScreenClick);
        }

        protected function onPizzaClick(evt:MouseEvent):void
        {
            trace("Pizza button clicked!");
        }

        protected function onTacosClick(evt:MouseEvent):void
        {
            trace("Tacos button clicked!");
        }

        protected function onScreenClick(event:MouseEvent):void
        {
            trace("---- blocking events ----");
            screen = new Sprite();
            screen.graphics.beginFill(0, 0);
            screen.graphics.drawRect(0, 0, stage.stageWidth, stage.stageHeight);
            // TODO: add stage resize listeners
            screen.graphics.endFill();
            // TODO ensure that screen stays on top
            addChild(screen);
            //TODO: optionally, handle all events and cancel them
        }
    }

}

import flash.display.Sprite;
import flash.text.TextField;
import flash.display.Shape;
import flash.text.TextFormat;
import flash.events.MouseEvent;
import flash.filters.BevelFilter;

class TestButton extends Sprite
{
    protected var bg:Shape;
    protected var label:TextField;
    public function TestButton(w:Number, h:Number, labelText:String)
    {
       bg = new Shape(); 
       addChild(bg);
       bg.graphics.beginFill(0xa0a0a0);
       bg.graphics.drawRect(0, 0, w, h);
       bg.graphics.endFill();

       label = new TextField;
       addChild(label);
       label.defaultTextFormat = new TextFormat("__sans", 11, 0, true, false, false, null, null, "center");
       label.width = w;
       label.height = h;
       label.text = labelText;
       label.y = (h - label.textHeight)/2 - 2;
       buttonMode = true;
       mouseChildren = false;

       addEventListener(MouseEvent.MOUSE_DOWN, onMouseDown);
       addEventListener(MouseEvent.MOUSE_UP, onMouseUp);
    }

    protected function onMouseDown(evt:MouseEvent):void
    {
       bg.filters = [new BevelFilter(4, 255, 0xffffff, 0.5, 0, 0.5, 1, 1, 1, 3)];
       label.x++;
       label.y++;
    }

    protected function onMouseUp(evt:MouseEvent):void
    {
       bg.filters = [new BevelFilter(-4, 255, 0xffffff, 0.5, 0, 0.5, 1, 1, 1, 3)];
       label.x--;
       label.y--;
    }
}