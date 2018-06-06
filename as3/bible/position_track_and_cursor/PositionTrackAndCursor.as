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
    public class PositionTrackAndCursor extends Sprite{
        public var group:Sprite = new Sprite;
        public function PositionTrackAndCursor(){
            var leftEye:Eye = new Eye(60);
            var rightEye:Eye = new Eye(60);
            leftEye.y = rightEye.y = stage.stageHeight / 2;
            leftEye.x = stage.stageWidth * 1/3;
            rightEye.x = stage.stageWidth * 2/3;
            addChild(leftEye);
            addChild(rightEye);
        }
    }
}

import flash.display.Sprite;
import flash.events.MouseEvent;
import flash.display.Shape;
import flash.events.Event;

internal class Eye extends Sprite
{
    protected var pupil:Shape;
    public function Eye(size:Number) {
        graphics.lineStyle(3);
        graphics.beginFill(0xffffff); 
        graphics.drawCircle(0, 0, size);
        graphics.endFill();

        pupil = new Shape();
        addChild(pupil);
        pupil.graphics.lineStyle();
        pupil.graphics.beginFill(0x603030);
        // attach pupil to eye circle, so when you rotate pupil
        // it rotates along eye circle
        pupil.graphics.drawCircle(0.75 * size, 0, size/4);

        scaleY = 2;
        addEventListener(Event.ADDED_TO_STAGE, onAddedToStage);
    }

    protected function onAddedToStage(evt:Event):void{
        stage.addEventListener(MouseEvent.MOUSE_MOVE, onMouseMove);
    }
    protected function onMouseMove(evt:MouseEvent):void {
        // this.mouseX is set eveny you add listener to stage.
        pupil.rotation = Math.atan2(this.mouseY, this.mouseX) * 180 / Math.PI; 
    }

}