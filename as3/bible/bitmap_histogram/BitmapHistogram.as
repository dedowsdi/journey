package
{
    import flash.display.Sprite;
    import flash.display.BitmapData;
    import flash.media.Video;
    import flash.display.Shape;
    import flash.media.Camera;
    import flash.events.Event;
    import flash.display.BlendMode;
    import flash.display.Graphics;
    import flash.display.Loader;
    import flash.net.URLRequest;

    // draw a live histogram over the webcam’s image
    [SWF(width="500", height="500", frameRate="20")]
    public class BitmapHistogram extends Sprite{
        protected const COLORS:Vector.<uint> = new <uint>[0xff0000, 0x00ff00, 0x0000ff];
        protected const SCALE:Number = 120/256;
        protected var bmp:BitmapData;
        protected var hstR:Shape, hstG:Shape, hstB:Shape;
        protected var allHstShape:Vector.<Shape>;
        protected var loader:Loader;

        public function BitmapHistogram(){
            loader = new Loader();
            loader.load(new URLRequest("monkey.png"));
            loader.contentLoaderInfo.addEventListener(Event.COMPLETE, loaded);
            this.addChild(loader);

            bmp = new BitmapData(stage.stageWidth, stage.stageHeight);
            bmp.draw(loader);

            var hstbox:Sprite = new Sprite();
            addChild(hstbox);
            hstbox.x = 0;
            hstbox.y = 100;
            hstbox.graphics.lineStyle(0, 0xffffff);
            hstbox.graphics.beginFill(0, 0.5);
            hstbox.graphics.drawRect(0, 0, 500, 150);

            hstR = new Shape();
            hstG = new Shape();
            hstB = new Shape();
            allHstShape = new <Shape>[hstR, hstG, hstB];

            for each(var hstShape:Shape in allHstShape)
            {
                hstbox.addChild(hstShape);
                hstShape.y = hstbox.height - 10;
                hstShape.blendMode = BlendMode.ADD;
                hstShape.rotationY = 36;
                hstShape.scaleY = -1;
            }

            hstR.x = 10;
            hstG.x = 160;
            hstB.x = 310;
        }

        protected function loaded(evt:Event):void
        {
            bmp.draw(loader);
            display();
        }

        protected function display():void
        {
            // historgram return four histograms, one for each channel, each
            // with 256 frequencies, indicating the number of pixels found in
            // the bitmap with the corresponding brightness(0-255).
            var allHstData:Vector.<Vector.<Number>> = bmp.histogram(bmp.rect);

            var PIXELCOUNT:int = bmp.width * bmp.height;

            for(var index:int = 0; index < allHstShape.length; index++)
            {
                var hstData:Vector.<Number> = allHstData[index];
                var g:Graphics = allHstShape[index].graphics;
                g.clear();
                g.beginFill(COLORS[index]);

                for(var j:int = 0; j < hstData.length; j++)
                {
                    g.drawRect(j * SCALE, 0, SCALE, hstData[j]/PIXELCOUNT * 1500 * SCALE);
                }
            }
        
        }
    }
}