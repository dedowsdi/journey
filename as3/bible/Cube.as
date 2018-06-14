package
{
    import flash.display.Sprite;
    import flash.events.MouseEvent;
    import flash.display.StageQuality;
    import flash.geom.Point;

    public class Cube extends Sprite{
        public function Cube(){
            stage.quality = StageQuality.MEDIUM;
            this.stage.addEventListener(MouseEvent.CLICK, onClick);
        }

        protected function onClick(evt:MouseEvent):void
        {
            var cube:MyCube = new MyCube(20);
            cube.x = evt.localX;
            cube.y = evt.localY;
            addChild(cube);
            var normalizedMousePosition2D:Point  = new Point(
                stage.mouseX / stage.stageWidth * 2 - 1,
                stage.mouseY / stage.stageHeight * 2 - 1
            );
            cube.angularVelocity.x = Math.pow(normalizedMousePosition2D.y, 3) * 10;
            cube.angularVelocity.y = Math.pow(normalizedMousePosition2D.x, 3) * 10;
        }
    }
}

import flash.display.Sprite;
import flash.display.BlendMode;
import flash.geom.Vector3D;
import flash.events.Event;

class MyCube extends Sprite{
    public var angularVelocity:Vector3D;
    public function MyCube(size:Number = 50, ang:Vector3D = null){

        for(var side:int = 0; side < 6; side++)
        {
            var face:Sprite = new Sprite();
            // there is no depth in flash 3d.
            face.blendMode = BlendMode.MULTIPLY;
            var brightness:int = 0x80 + 0x80 * Math.random();
            var color:int = brightness << 16 | brightness << 8 | brightness;
            face.graphics.beginFill(color, 1);
            face.graphics.drawRect(-size/2, -size/2, size, size);
            face.graphics.endFill();
            addChild(face);
        }

        getChildAt(0).rotationY = 90; // left
        getChildAt(0).x = -size/2;
        getChildAt(1).rotationY = -90; // right
        getChildAt(1).x = size/2;
        getChildAt(2).z = -size/2; // front
        getChildAt(3).rotationY = 180; // back
        getChildAt(3).z = size/2;
        getChildAt(4).rotationX = 90; // bottom
        getChildAt(4).y = size/2;
        getChildAt(5).rotationX = -90; // top
        getChildAt(5).y = -size/2;
        
        this.angularVelocity = ang ? ang : new Vector3D();
        this.addEventListener(Event.ENTER_FRAME, onEnterFrame);
    }

    protected function onEnterFrame(evt:Event):void
    {
        this.rotationX += angularVelocity.x;
        this.rotationY += angularVelocity.y;
        this.rotationZ += angularVelocity.z;
    }
}

