package
{
    import flash.display.Sprite;
    import flash.events.Event;
    import flash.geom.Matrix;
    import flash.display.GradientType;
    import flash.filters.BlurFilter;

    public class BlurFilter extends Sprite{
        protected const NUM_COPIES:int = 28;
        public function BlurFilter(){
            var W:Number = stage.stageWidth;
            var H:Number = stage.stageHeight;

            for(var i:int = 0; i < NUM_COPIES; i++)
            {
                var s:Sprite = new Sprite;
                var m:Matrix = new Matrix;
                m.createGradientBox(40, 40, 0, -20, -20);
                s.graphics.beginGradientFill(GradientType.RADIAL, [0xE76921, 0x8F1F08],
                    [1,1], [0,255], m, null, null, 0.6
                );
                s.graphics.drawCircle(0, 0, 20);
                s.graphics.endFill();
                s.x = (Math.random() - 0.5) * 200 + 0.5 * W;
                s.y = (Math.random() - 0.5) * 200 + 0.5 * H;
                s.z = -(i - NUM_COPIES/2) * W/NUM_COPIES; // -0.5*W to 0.5*W
                s.filters = [new flash.filters.BlurFilter(0, 0, 2)];
                addChild(s);
            }

            this.addEventListener(Event.ENTER_FRAME, onEnterFrame);
        }

        protected function onEnterFrame(evt:Event):void
        {
            var focusZ:Number = stage.mouseX - stage.stageWidth/2;
            for(var i:int = 0; i < numChildren; i++)
            {
                var s:Sprite = Sprite(getChildAt(i));
                var blurFilter:flash.filters.BlurFilter  = new flash.filters.BlurFilter();
                var blurAmount:Number = 0.05 * Math.abs(focusZ - s.z);
                blurFilter.blurX = blurAmount;
                blurFilter.blurY = blurAmount;
                s.filters = [blurFilter];
            }
        }
    }

}