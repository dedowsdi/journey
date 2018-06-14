package
{
    import flash.display.BitmapData;
    import flash.display.Bitmap;
    import flash.geom.Point;
    import flash.geom.Rectangle;
    import flash.utils.Timer;
    import flash.events.TimerEvent;
    import flash.display.Sprite;

    public class Test extends Sprite
    {
        public var bmd:BitmapData = new BitmapData(100, 80, false, 0x00CCCCCC);
        public var tim:Timer = new Timer(20);

        public function Test()
        {
            var bitmap:Bitmap = new Bitmap(bmd);
            addChild(bitmap);

            tim.start();
            tim.addEventListener(TimerEvent.TIMER, timerHandler);
        }
 
        public function timerHandler(event:TimerEvent):void {
            var randomNum:Number = Math.floor(Math.random() * int.MAX_VALUE);
            dissolve(randomNum);
        }

        public function dissolve(randomNum:Number):void {
            var rect:Rectangle = bmd.rect;
            var pt:Point = new Point(0, 0);
            var numberOfPixels:uint = 100;
            var red:uint = 0xFFFF0000;

            // dissolve until gray region is 0 with and height
            bmd.pixelDissolve(bmd, rect, pt, randomNum, numberOfPixels, red);
            var grayRegion:Rectangle = bmd.getColorBoundsRect(0xFFFFFFFF, 0x00CCCCCC, true);
            if(grayRegion.width == 0 && grayRegion.height == 0 ) {
                tim.stop();
            }
        }
            
    }
}