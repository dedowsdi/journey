package
{
    import flash.display.Sprite;
    import flash.events.Event;
    import flash.display.BitmapData;
    import flash.display.Loader;
    import flash.net.URLRequest;
    import flash.system.LoaderContext;
    import flash.display.Bitmap;
    import flash.display.BlendMode;
    import flash.display.BitmapDataChannel;
    import flash.filters.BlurFilter;
    import flash.geom.Point;

    [SWF(frameRate="24", backgroundColor="#777777", width = "1500", height = "800")]
    public class BitmapNoise extends Sprite{
        protected var bmp0:BitmapData;
        protected var bmp1:BitmapData;
        protected var bmp2:BitmapData;
        protected var bitmap0:Bitmap;
        protected var bitmap1:Bitmap;
        protected var bitmap2:Bitmap;

        protected var seed:int;
        protected var image:Loader;
        public function BitmapNoise(){
            image = new Loader();
            this.addChild(image);
            image.contentLoaderInfo.addEventListener(Event.COMPLETE, onLoad);
            image.load(new URLRequest("caviar.jpg"), new LoaderContext(true));
        }

        protected function onLoad(evt:Event):void
        {
            var src:BitmapData = Bitmap(image.content).bitmapData;

            bitmap0 = createBitmap(src.width + 10, 0);
            bmp0 = bitmap0.bitmapData;
            bitmap0.blendMode = BlendMode.MULTIPLY;

            bitmap1 = createBitmap((src.width + 10), src.height + 10);
            bmp1 = bitmap1.bitmapData;

            bitmap2 = createBitmap((src.width + 10) * 3, (src.height + 10) * 2);
            bmp2 = bitmap2.bitmapData;

            addEventListener(Event.ENTER_FRAME, onEnterFrame);
        }

        protected function createBitmap(x:Number, y:Number):Bitmap{
            var bmp:BitmapData = new BitmapData(image.width, image.height, true);
            var bitmap:Bitmap = new Bitmap(bmp);
            addChild(bitmap);
            bitmap.x = x;
            bitmap.y = y;

            return bitmap;
        }

        protected function onEnterFrame(evt:Event):void
        {
            bmp0.fillRect(bmp0.rect, 0);
            bmp0.noise(++seed, 200, 255,  BitmapDataChannel.BLUE, true);
            bmp0.applyFilter(bmp0, bmp0.rect, new Point(), new BlurFilter(4,4,2));
            image.alpha = (seed % 2) ? 0.95 : 1;

            bmp1.noise(++seed, 200, 255,  BitmapDataChannel.BLUE, true);
            
            bmp2.noise(++seed, 200, 255,  BitmapDataChannel.BLUE, true);
            bmp2.applyFilter(bmp2, bmp2.rect, new Point(), new BlurFilter(4,4,2));
        }
    }
}