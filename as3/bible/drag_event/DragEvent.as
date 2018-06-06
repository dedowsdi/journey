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
    public class DragEvent extends Sprite{
        public var group:Sprite = new Sprite;
        public function DragEvent(){
            var circle:DraggableCircle = new DraggableCircle();
            circle.x = circle.y = 100;
            addChild(circle);
        }
    }
}

import flash.display.Sprite;
import flash.events.MouseEvent;
import mx.events.DragEvent;

internal class DraggableCircle extends Sprite{
    public function DraggableCircle(){
        graphics.beginFill(0, 0.5);
        graphics.drawCircle(0, 0, 50);
        graphics.endFill();
        addEventListener(MouseEvent.MOUSE_DOWN, onStartDrag);
        this.buttonMode = true;
    }

    protected function onStartDrag(evt:MouseEvent):void{
        if(evt && evt.shiftKey){
            cloneAndDrag();
        }else{
            startDrag();
            this.stage.addEventListener(MouseEvent.MOUSE_UP, onStopDrag);
        }
    }

    protected function onStopDrag(evt:MouseEvent):void{
        this.stage.removeEventListener(MouseEvent.MOUSE_UP, onStopDrag);
        stopDrag();
    }

    protected function cloneAndDrag():void{
        var clone:DraggableCircle = new DraggableCircle;
        clone.x = this.x;
        clone.y = this.y;
        this.parent.addChild(clone);
        clone.onStartDrag(null);
    }

}