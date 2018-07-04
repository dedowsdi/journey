package
{
    import flash.display.BitmapData;
    import flash.display.Bitmap;
    import flash.geom.Point;
    import flash.geom.Rectangle;
    import flash.utils.Timer;
    import flash.events.TimerEvent;
    import flash.display.Sprite;

    public class BitmapDissolve extends Sprite
    {
        public var bmp0:BitmapData = new BitmapData(100, 80, false, 0x00CCCCCC);
        public var bmp1:BitmapData = new BitmapData(100, 80, false, 0x00FF0000);
        public var tim:Timer = new Timer(20);
        public var seed0:Number = Math.random() * int.MAX_VALUE;
        public var seed1:Number = Math.random() * int.MAX_VALUE;

        public var bmp2:BitmapData = new BitmapData(100, 80, false, 0xff000000);
        public var bmp3:BitmapData = new BitmapData(100, 80, false, 0xff00ff00);

        public var count:int = 0;

        public function BitmapDissolve()
        {
            var bitmap0:Bitmap = new Bitmap(bmp0);
            addChild(bitmap0);

            var bitmap1:Bitmap = new Bitmap(bmp1);
            addChild(bitmap1);
            bitmap1.x = 150;

            var bitmap2:Bitmap = new Bitmap(bmp2);
            addChild(bitmap2);
            bitmap2.x = 300;

            tim.start();
            tim.addEventListener(TimerEvent.TIMER, timerHandler);
        }
 
        public function timerHandler(event:TimerEvent):void {
            var randomNum:Number = Math.floor(Math.random() * int.MAX_VALUE);
            dissolve(randomNum);
        }

        public function dissolve(randomNum:Number):void {
            var rect:Rectangle = bmp0.rect;
            var pt:Point = new Point(0, 0);
            var numberOfPixels:uint = 100;
            var red:uint = 0xFFFF0000;

            // dissolve until gray region is 0 with and height, use a random seed everytime just for demostration.
            bmp0.pixelDissolve(bmp0, rect, pt, randomNum, numberOfPixels, red);
            var grayRegion:Rectangle = bmp0.getColorBoundsRect(0xFFFFFFFF, 0x00CCCCCC, true);
            if(grayRegion.width == 0 && grayRegion.height == 0 ) {
                tim.stop();
            }

            if(count++ >= 80)
                return;
            trace("step " + count);

            // returned seed should be used in next round
            seed0 = bmp1.pixelDissolve(bmp1, rect, new Point(0, 0), seed0, 100, 0xff0000ff);
            seed1 = bmp2.pixelDissolve(bmp3, rect, new Point(0, 0), seed1, 100, 0xff0000ff);
        }
            
    }
}