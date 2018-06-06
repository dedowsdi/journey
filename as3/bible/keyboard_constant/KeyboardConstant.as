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

    // hold key to move brick every frame
    [SWF(backgroundColr="0x000000", frameRate="4")]
    public class KeyboardConstant extends Sprite{
        protected var hero: Hero;
        protected var keys:Array;
        protected const MAX_KEY:int =128;

        public function KeyboardConstant(){
            stage.addEventListener(KeyboardEvent.KEY_DOWN, onKey);
            stage.addEventListener(KeyboardEvent.KEY_UP, onKey);
            keys = new Array(MAX_KEY);

            hero = new Hero();
            addChild(hero);
            hero.x = stage.stageWidth / 2;
            hero.y = stage.stageHeight /2;
            addEventListener(Event.ENTER_FRAME, onEnterFrame)
        }

        protected function onKey(evt:KeyboardEvent):void
        {
            if(evt.keyCode >= MAX_KEY)
                return;

            keys[evt.keyCode] = (evt.type == KeyboardEvent.KEY_DOWN);
        }

        protected function onEnterFrame(evt:Event):void
        {
           if(keys[Keyboard.UP]) hero.y -= hero.height; 
           if(keys[Keyboard.DOWN]) hero.y += hero.height; 
           if(keys[Keyboard.LEFT]) hero.x -= hero.height; 
           if(keys[Keyboard.RIGHT]) hero.x += hero.height; 
        }
    }
}

import flash.display.Shape;

internal class Hero extends Shape
{
    public function Hero ()
    {
        graphics.beginFill(0x10c010);
        graphics.drawRect(0, 0, 12, 30);
        graphics.endFill();
    }
}
