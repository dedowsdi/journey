package
{
    import flash.display.Sprite;
    import flash.utils.setInterval;

    public class PieMask extends Sprite
    {
        public var pie:Sprite;
        public var pieMask:MyPieMask;

        public function PieMask()
        {
            pie = new Sprite();
            this.addChild(pie);
            pie.x = pie.y = 200;
            pie.graphics.lineStyle(5, 0xffff0000);
            pie.graphics.drawCircle(0, 0, 100);

            pieMask  = new MyPieMask(110, 0);
            this.mask = pieMask;
            pie.addChild(pieMask);

            setInterval(onPie,  30);
        }

        public function onPie():void{
            pieMask.end += 1;
        }
    }

}

import flash.display.Sprite;

class MyPieMask extends Sprite
{
    public var start:Number;
    public var _end:Number;
    public var radius:Number;

    public function MyPieMask(radius:Number = 1, start:Number = 0)
    {
        this.start = start;
        this._end = start;
        this.radius = radius;
    }

    public function get end():Number{
        return _end;
    }

    public function set end(v:Number):void{
        drawSlice(_end, v);
        this._end = v;
    }

    // draw triangle slice
    protected function drawSlice(startAngle:Number, endAngle:Number):void{
        var startRadian:Number = startAngle * Math.PI/ 180;
        var endRadian:Number = endAngle * Math.PI/ 180;

        var x1:Number = radius *Math.sin(startRadian);
        var y1:Number = -radius *Math.cos(startRadian);
        var x2:Number = radius *Math.sin(endRadian);
        var y2:Number = -radius *Math.cos(endRadian);
        trace(x1 + ":" + y1);

        this.graphics.moveTo(0,0);
        this.graphics.beginFill(0x000000, 0.5);
        this.graphics.lineTo(x1,y1);
        this.graphics.lineTo(x2,y2);
        this.graphics.endFill();
    }

}