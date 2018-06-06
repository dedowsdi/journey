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
    import flash.display.Shape;

    // weird thing happen if you turn off mouseChildren of root object, the root object itself will stop accept mouse event.
    public class AutoSimpleButton extends Sprite{
        public var group:Sprite = new Sprite;
        public function AutoSimpleButton(){
            var btn:SimpleButton = new SimpleButton;
            btn.upState = new ButtonState();
            btn.overState = new Shape();
            Shape(btn.overState).graphics.copyFrom(Shape(btn.upState).graphics);
            btn.hitTestState = btn.overState;
            this.addChild(btn);
            btn.useHandCursor = true;

            btn.addEventListener(MouseEvent.CLICK, onClick);
        }

        public function onClick(evt:MouseEvent):void
        {
            trace("click");
        }
    }
}

import flash.display.Shape;

internal class ButtonState extends Shape
{
    public function ButtonState()
    {
        graphics.beginFill(0x0000ff);
        graphics.drawRect(0, 0, 100, 100);
        graphics.endFill();
    }
}