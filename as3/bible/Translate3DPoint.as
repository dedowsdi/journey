// transform diagonal vertices of cube to 2d
package
{
    import flash.display.Sprite;
    import flash.geom.Vector3D;
    import flash.display.Shape;
    import flash.events.MouseEvent;
    import flash.events.Event;
    import flash.geom.Point;

    public class Translate3DPoint extends Sprite{
        protected var cube:MyCube;
        protected var angularVelocity:Vector3D;
        protected var layers:Vector.<Shape> = new Vector.<Shape>;
        protected var colors:Vector.<uint> = new <uint>[0xD97C2B, 0x496B73];
        protected var firstFrame:Boolean = true;
        public function Translate3DPoint(){
            cube = new MyCube(200);
            cube.x = stage.stageWidth/2;
            cube.y = stage.stageHeight/2;
            addChild(cube);

            for(var i:int = 0; i < colors.length; i++)
            {
                layers[i] = new Shape();
                addChild(layers[i]);
            }
            stage.addEventListener(MouseEvent.CLICK, onClick);
            addEventListener(Event.ENTER_FRAME, onEnterFrame);
            onClick(null);
        }

        protected function onClick(evt:MouseEvent):void
        {
            var r:Function = function():Number {return Math.random() * 8 - 4};
            angularVelocity = new Vector3D(r(), r(), r());
            firstFrame = true;
            for(var i:int = 0; i < layers.length; i++)
            {
                layers[i].graphics.clear();
                layers[i].graphics.lineStyle(0, colors[i], 1);
            }
        }

        protected function onEnterFrame(evt:Event):void
        {
            cube.rotationX += angularVelocity.x;
            cube.rotationY += angularVelocity.y;
            cube.rotationZ += angularVelocity.z;

            for(var i:int = 0; i < layers.length; i++)
            {
                var p:Point = cube.local3DToGlobal(cube.vertices[i]);
                if(firstFrame){
                    layers[i].graphics.moveTo(p.x, p.y);
                }else{
                    layers[i].graphics.lineTo(p.x, p.y);
                }
            }
            firstFrame = false;
        }
    }
}

import flash.display.Sprite;
import flash.display.BlendMode;
import flash.geom.Vector3D;
import flash.events.Event;
import flash.display.LineScaleMode;

class MyCube extends Sprite{
    public var vertices:Vector.<Vector3D>;
    public function MyCube(size:Number = 50){

        var s2:Number = size/2;
        for(var side:int = 0; side < 6; side++)
        {
            var face:Sprite = new Sprite();
            face.blendMode = BlendMode.MULTIPLY;
            face.graphics.lineStyle(8,0, 0.1, false, LineScaleMode.NORMAL);
            face.graphics.drawRect(-s2, -s2, size, size);
            addChild(face);
        }

        vertices = new Vector.<Vector3D>;
        vertices.push(new Vector3D(-s2, -s2, -s2));
        vertices.push(new Vector3D(s2, s2, s2));

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
    }

}

