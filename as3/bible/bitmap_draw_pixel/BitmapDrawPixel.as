package {

import flash.display.*;
import flash.geom.*;
import flash.events.MouseEvent;

    public class BitmapDrawPixel extends Sprite 
    {
        protected const AVG_RADIUS:Number = 30;
        protected var bmp:BitmapData;

        public function BitmapDrawPixel() 
        {
            bmp = new BitmapData(stage.stageWidth, stage.stageHeight);
            var bitmap:Bitmap = new Bitmap(bmp);
            addChild(bitmap);
            stage.addEventListener(MouseEvent.CLICK, onClick);
        }

        protected function onClick(event:MouseEvent):void 
        {
            bmp.lock();

            var radius:Number = (Math.random() - 0.5) * AVG_RADIUS + AVG_RADIUS;
            var center:Point = new Point(event.localX, event.localY);
            var bounds:Rectangle = new Rectangle(center.x, center.y, 0, 0);
            bounds.inflate(2*radius, 2*radius);
            bounds = bounds.intersection(bmp.rect);
            var p:Point = new Point();

            for (p.y = int(bounds.top); p.y < bounds.bottom; p.y++) 
            {
                for (p.x = int(bounds.left); p.x < bounds.right; p.x++) 
                {
                    var dist:Number = Point.distance(p, center);
                    if (dist < radius) 
                    {
                        var alpha:uint = 0xff * (1 - dist / radius);
                        bmp.setPixel32(p.x, p.y, alpha << 24);
                    }
                }
            }

            bmp.unlock(bounds);
        }
    }
}