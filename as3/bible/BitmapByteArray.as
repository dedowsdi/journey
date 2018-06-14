package
{
    import flash.display.Sprite;
    import flash.display.Loader;
    import flash.net.URLRequest;
    import flash.events.Event;
    import flash.display.BitmapData;
    import flash.display.Bitmap;
    import flash.utils.ByteArray;
    import flash.system.LoaderContext;
    import flash.display.LoaderInfo;

    public class BitmapByteArray extends Sprite{
        public function BitmapByteArray(){
            var l:Loader = new Loader();
            l.load(new URLRequest("test.png"));
            l.contentLoaderInfo.addEventListener(Event.COMPLETE, onComplete);
        }

        protected function onComplete(evt:Event):void
        {
            var src:BitmapData = Bitmap(LoaderInfo(evt.target).content).bitmapData;
            var bmp:BitmapData = new BitmapData(src.width, src.height, true);
            bmp.draw(src);

            // setting the alpha channel doesn't work if the image isn't transparent,
            // so you copy the bitmapdata into a new transparent BitmapData.
            var bytes:ByteArray = src.getPixels(src.rect);
            for(var y:int = 0; y < bmp.height; y+=2)
            {
                for(var x:int = 0; x < bmp.width; x++)
                {
                    // does flash always use big endian? is it safe to assume this?
                    bytes[ y * bmp.width * 4 + x*4 ] = 0x0;
                }
            }

            // you must rewind the bytearray first, for some reason
            bytes.position = 0;
            bmp.setPixels(bmp.rect, bytes);
            var bitmap:Bitmap = new Bitmap(bmp);
            addChild(bitmap);
        }
    }
}