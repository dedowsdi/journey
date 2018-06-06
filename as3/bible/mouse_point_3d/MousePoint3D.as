// you can paint on a rotating 3d canvas.
package
{
    import flash.display.Sprite;
    import flash.events.Event;

    public class MousePoint3D extends Sprite{
        protected var canvas:Canvas;
        public function MousePoint3D(){
            canvas = new Canvas(400, 300);
            addChild(canvas);
            canvas.x = stage.stageWidth / 2;
            canvas.y = stage.stageHeight / 2;
            canvas.rotationY = 25;
            addEventListener(Event.ENTER_FRAME, onEnterFrame);
        }

        protected function onEnterFrame(evt:Event):void
        {
            canvas.rotationY += 0.2;
            canvas.rotationX += 0.04;
        }
    }
}


import flash.display.Sprite;
import flash.display.Shape;
import flash.events.MouseEvent;
import flash.geom.Rectangle;

class Canvas extends Sprite
{
    protected var fg:Shape, bg:Shape;
    protected var ink:int = 0;

    public function Canvas(w:Number, h:Number)
    {
        bg = new Shape();
        bg.graphics.beginFill(0xe0e0e0, 1);
        bg.graphics.drawRect(0, 0, w, h);
        bg.graphics.endFill();
        bg.x = -w/2;
        bg.y = -h/2;
        addChild(bg);

        fg = new Shape();
        fg.x = -w/2;
        fg.y = -h/2;
        addChild(fg);
        //@TODO understand this
        fg.scrollRect = new Rectangle(-w/2, -h/2, w, h);
        addEventListener(MouseEvent.MOUSE_MOVE, onMouseMove);
    }

    protected function onMouseMove(evt:MouseEvent):void
    {
        if(evt.buttonDown){
            ink++;
            fg.graphics.lineTo(evt.localX, evt.localY);
        }else{
            if(ink > 500){
                fg.graphics.clear();
                ink = 0;
            }
            fg.graphics.lineStyle(32, 0xffffff * Math.random(), 0.4);
            fg.graphics.moveTo(evt.localX, evt.localY);
        }
    }
}