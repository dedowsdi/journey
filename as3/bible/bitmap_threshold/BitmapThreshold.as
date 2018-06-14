package
{
    import flash.display.Sprite;
    import flash.display.Loader;
    import flash.events.Event;
    import flash.net.URLRequest;
    import flash.display.Bitmap;
    import flash.display.BitmapData;
    import flash.geom.Point;

    public class BitmapThreshold extends Sprite{

        public var bmp:BitmapData;
        public var loader:Loader;

        public function BitmapThreshold(){
            loader = new Loader()
            loader.load(new URLRequest("monkey.png"));
            loader.contentLoaderInfo.addEventListener(Event.COMPLETE, loaded);
            this.addChild(loader);
        }

        protected function loaded(evt:Event):void
        {
            bmp = Bitmap(loader.content).bitmapData;
            display();
        }

        public function display():void{

            // change any color value that's larger than the combine of each most frequency color(non 0) to 0
            var hst:Vector.<Vector.<Number>> = bmp.histogram(bmp.rect);
            var maxValue:Vector.<int> = new Vector.<int>(3, true); // max color value of the most frequency color in  r g b channel
            var maxFrequency:Vector.<Number> = new Vector.<Number>(3, true); // frequency of the most frequnecy color in each channel

            for(var channel:int = 0; channel < 3; channel++)
            {
                for(var i:int = 1; i < 256; i++) // excludes 0
                {
                    var frequency:Number = hst[channel][i];
                    trace(i, frequency);
                    if(frequency > maxFrequency[channel]){
                        maxFrequency[channel] = frequency;
                        maxValue[channel] = i
                    }
                }
            }

            // set threshold to max r g b
            var threshold:uint = maxValue[0] << 16 | maxValue[1] << 8 | maxValue[2];
            trace(threshold.toString(16));

            // If ((pixelValue & mask) operation (threshold & mask)), then set the pixel to color
            bmp.threshold(bmp, bmp.rect, new Point(), ">", threshold, 0xff000000, 0x00ffffff, true);
        }
    }
}