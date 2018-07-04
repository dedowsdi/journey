package {
    import flash.display.Sprite;
    import flash.filters.BitmapFilter;
    import flash.filters.BitmapFilterQuality;
    import flash.filters.BitmapFilterType;
    import flash.filters.GradientBevelFilter;

    public class Test extends Sprite {
        private var bgColor:uint     = 0xCCCCCC;
        private var size:uint        = 80;
        private var offset:uint      = 50;
        private var distance:Number  = 5;
        private var angleInDegrees:Number = 225; // opposite 45 degrees
        private var colors:Array     = [0xFFFFFF, 0xCCCCCC, 0x000000];
        private var alphas:Array     = [1, 0, 1];
        private var ratios:Array     = [0, 128, 255];
        private var blurX:Number     = 8;
        private var blurY:Number     = 8;
        private var strength:Number  = 2;
        private var quality:Number   = BitmapFilterQuality.HIGH
        private var type:String      = BitmapFilterType.INNER;
        private var knockout:Boolean = true;

        public function Test() {
            draw();
            var filter:BitmapFilter = getBitmapFilter();
            var myFilters:Array = new Array();
            myFilters.push(filter);
            filters = myFilters;
        }

        private function getBitmapFilter():BitmapFilter {
            return new GradientBevelFilter(distance,
                                           angleInDegrees,
                                           colors,
                                           alphas,
                                           ratios,
                                           blurX,
                                           blurY,
                                           strength,
                                           quality,
                                           type,
                                           knockout);
        }

        private function draw():void {
            graphics.beginFill(bgColor);
            graphics.drawRect(offset, offset, size, size);
            graphics.endFill();
        }
    }
}