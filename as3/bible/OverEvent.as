/*
roll_* doesn't bubble. It's purpose is to simplify mouse event with children, treats
displayobject and children as a whole if you doesn't add listener to children.

move cursor from children to parent:
    dispatch roll_out to children
    dispatch move_out to children
    dispatch move_over to parent

move cursor from parent to children:
    dispatch roll_over to children
    dispatch move_out to parent
    dispatch move_over to children
*/
package
{
    import flash.display.Sprite;
    import flash.display.SimpleButton;
    import flash.geom.Point;
    import flash.events.MouseEvent;
    import flash.display.DisplayObjectContainer;
    import flash.display.DisplayObject;
    import com.tuto.SimpleTextButton;

    // weird thing happen if you turn off mouseChildren of root object, the root object itself will stop accept mouse event.
    public class OverEvent extends Sprite{
        public var group:Sprite = new Sprite;
        public function OverEvent(){
            // use ROLL_OVER and ROLL_OUT on the left red one
            var a:NestedCircle = new NestedCircle(true, 0xff0000);
            // use MOUSE_OVER and MOUSE_OUT on the right blue one
            var b:NestedCircle = new NestedCircle(false, 0x0000ff);
            a.x = 100;
            b.x = 250;
            a.y = b.y = 150;
            addChild(a);
            addChild(b);
        }
    }
}

import flash.display.Sprite;
import flash.display.Shape;
import flash.events.MouseEvent;

class NestedCircle extends Sprite{
    public var child:NestedCircle;
    protected var stroke:Shape;

    public function NestedCircle(useRoll:Boolean, color:uint = 0, 
        size:Number = 60, isChild:Boolean = false){
        
        this.graphics.beginFill(color, 0.25);
        this.graphics.drawCircle(0, 0, size);
        this.graphics.endFill();

        stroke = new Shape();
        addChild(stroke);
        stroke.graphics.lineStyle(5, 0xffff00);
        stroke.graphics.drawCircle(0, 0, size);
        stroke.visible = false;

        if(useRoll){
            addEventListener(MouseEvent.ROLL_OVER, handler)
            addEventListener(MouseEvent.ROLL_OUT, handler)
        }else{
            addEventListener(MouseEvent.MOUSE_OVER, handler)
            addEventListener(MouseEvent.MOUSE_OUT, handler)
        }

        if(!isChild){
            child = new NestedCircle(useRoll, color, size/2, true);
            addChild(child);
            child.y = -size;
        }
    }

    public function handler(evt:MouseEvent):void
    {
        switch(evt.type){
            case MouseEvent.MOUSE_OUT:
            case MouseEvent.ROLL_OUT:
                stroke.visible = false;
                evt.stopPropagation();
                break;

            case MouseEvent.MOUSE_OVER:
            case MouseEvent.ROLL_OVER:
                stroke.visible = true;
                evt.stopPropagation();
                break;
        }
    }
}