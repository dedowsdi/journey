package
{
    import flash.display.Sprite;
    import flash.events.MouseEvent;
    import flash.events.KeyboardEvent;
    import flash.geom.Point;
    import flash.display.Shape;

    public class Curve extends Sprite{
        private var pts:Vector.<Point>;
        private var curve:Shape;
        private var points:Shape;

        public function Curve(){
            points = new Shape();
            this.addChild(points);
            curve = new Shape();
            this.addChild(curve);

            pts = new Vector.<Point>;

            stage.addEventListener(MouseEvent.CLICK, onCLick);
            stage.addEventListener(KeyboardEvent.KEY_DOWN, onKeyDown);
        }

        protected function onCLick(evt:MouseEvent):void
        {
            var point:Point = new Point(stage.mouseX, stage.mouseY);

            if(pts.length > 0){
                var midpoint:Point = Point.interpolate(pts[pts.length - 1], point, 0.5);
                pts.push(midpoint);
                points.graphics.lineStyle(1, 0, 0.2);
                points.graphics.drawCircle(midpoint.x, midpoint.y, 5);

            }
            pts.push(point);

            points.graphics.lineStyle(1, 0, 0.5);
            points.graphics.drawCircle(point.x, point.y, 2);

            drawCurve();
        }

        protected function onKeyDown(evt:KeyboardEvent):void
        {
            pts = new Vector.<Point>;
            points.graphics.clear();
            curve.graphics.clear();

        }

        protected function drawCurve():void
        {
            if(pts.length < 4)
                return;
            
            curve.graphics.clear();
            curve.graphics.lineStyle(3);
            curve.graphics.moveTo(pts[1].x, pts[1].y);
            // throw 1st and the last, because we are drawing between midpoints
            var len:int = pts.length - 3 // i+2 < length-1
            for(var i:int = 1; i < len; i += 2)
            {
                curve.graphics.curveTo(pts[i+1].x, pts[i+1].y, pts[i+2].x, pts[i+2].y);
            }
        }
    }
}