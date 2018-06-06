/*
during bitmapData.draw(...)
    clipRect will be applied to both bitmapData and source.
    matrix will be applied to clipRect of source
    
    so draw() copy matrix * clipRect area of source to clipRect area of bitmapData.
    If clipRect doesn't exists in bitmapData, nothing will happen.
*/
package
{
    import flash.display.Sprite
    import flash.geom.Rectangle;
    import flash.display.BitmapData;
    import org.osmf.elements.ProxyElement;
    import flash.display.Loader;
    import flash.net.URLRequest;
    import flash.system.LoaderContext;
    import flash.events.Event;
    import flash.display.Bitmap;
    import flash.geom.Matrix;

    [SWF(frameRate="5", width="600", height="300")]
    public class BitmapDraw extends Sprite{
        protected const SIZE:Rectangle = new Rectangle(0, 0, 48, 48);
        protected var TOTALFRAMES:int;
        protected var bmp0:BitmapData;
        protected var bmp1:BitmapData;
        protected var filmstrip:Loader;
        protected var frame:int = 0;

        public function BitmapDraw(){
            filmstrip = new Loader();
            filmstrip.load(new URLRequest("monkey.png"), new LoaderContext(true));
            filmstrip.contentLoaderInfo.addEventListener(Event.COMPLETE, onLoad);
            addChild(filmstrip);

            bmp0 = new BitmapData(SIZE.width, SIZE.height);
            var bitmap0:Bitmap = new Bitmap(bmp0);
            addChild(bitmap0);
            bitmap0.y = SIZE.height + 10;

            bmp1 = new BitmapData(SIZE.width, SIZE.height);
            var bitmap1:Bitmap = new Bitmap(bmp1);
            addChild(bitmap1);
            bitmap1.y = SIZE.height * 2 + 20;
        }

        protected function onLoad(evt:Event):void
        {
            TOTALFRAMES = filmstrip.width / SIZE.width;
            addEventListener(Event.ENTER_FRAME, onEnterFrame);
        }

        protected function onEnterFrame(evt:Event):void
        {
            frame = ++frame %TOTALFRAMES;
            // highlight the are you're drawing
            graphics.clear();
            graphics.lineStyle(0, 0xff0000);
            graphics.drawRect(frame*SIZE.width, 0, SIZE.width, SIZE.height);
            // clear the bitmap
            bmp0.fillRect(bmp0.rect, 0);
            bmp1.fillRect(bmp1.rect, 0);
            // shift the filmstrip to the left to tput the right frame in place
            var shiftTransform:Matrix = new Matrix();
            shiftTransform.translate(-frame*SIZE.width, 0);
            //use SIZE to clip the shifted graphics to  the corrected position and size
            bmp0.draw(filmstrip, shiftTransform, null, null, SIZE);

            // you can only see this bmp when frame is 0. draw() simply draw
            var clipRect:Rectangle = 
                new Rectangle(SIZE.x + SIZE.width * frame, SIZE.y, SIZE.width, SIZE.height);
            bmp1.draw(filmstrip, null, null, null, clipRect);
        }

    }
}