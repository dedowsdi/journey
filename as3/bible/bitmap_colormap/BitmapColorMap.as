package
{
    import flash.display.Sprite;
    import flash.display.Loader;
    import flash.events.Event;
    import flash.net.URLRequest;
    import flash.display.Bitmap;
    import flash.display.BitmapData;
    import flash.geom.Point;
    import flash.text.TextField;
    import flash.events.MouseEvent;
    import flash.text.TextFormat;
    import mx.core.BitmapAsset;

    public class BitmapColorMap extends Sprite{

        public var bmp:BitmapData;
        public var loader:Loader;
        public var bitmap:Bitmap;
        protected var RLUT:Array, GLUT:Array, BLUT:Array;
        protected var tf:TextField;

        public function BitmapColorMap(){
            loader = new Loader()
            loader.load(new URLRequest("monkey.png"));
            loader.contentLoaderInfo.addEventListener(Event.COMPLETE, loaded);
            this.addChild(loader);

            RLUT = new Array(256);
            GLUT = new Array(256);
            BLUT = new Array(256);

            stage.addEventListener(MouseEvent.MOUSE_MOVE, onMouseMove);
            tf = new TextField();
            tf.width = 26;
            tf.height = 14;
            tf.x = (stage.stageWidth  - tf.width)/2;
            tf.y = (stage.stageHeight - tf.height)/2;

            tf.backgroundColor = 0;
            tf.background = true;
            tf.defaultTextFormat = new TextFormat("_typewriter", 10, 0xffffff);
            addChild(tf);
            
        }

        protected function onMouseMove(evt:MouseEvent):void
        {
            var newGamma:Number = (1 - stage.mouseY / stage.stageHeight) * 2.5;
            tf.text = newGamma.toFixed(1);
            for(var i:int = 0; i < 256; i++)
            {
                BLUT[i] = int(Math.pow(i / 256.0, newGamma) * 256)
                GLUT[i] = BLUT[i] << 8;
                RLUT[i] = GLUT[i] << 8;
            }
            bitmap.bitmapData.draw(loader);
            bitmap.bitmapData.paletteMap(bitmap.bitmapData, 
                bitmap.bitmapData.rect, new Point(0,0), RLUT, GLUT, BLUT);
        }

        protected function loaded(evt:Event):void
        {
            bmp = Bitmap(loader.content).bitmapData;
            var bmp2:BitmapData = new BitmapData(bmp.width, bmp.height, bmp.transparent);
            bmp2.copyPixels(bmp,bmp.rect, new Point(0, 0));
            bitmap = new Bitmap(bmp2);
            this.addChild(bitmap);
            bitmap.y = bitmap.height + 10;
        }

    }
}