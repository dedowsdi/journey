package
{
    import flash.display.Sprite;
    import flash.display.Sprite;
    import flash.events.Event;
    import flash.filters.DropShadowFilter;
    import flash.geom.Point;
    import flash.text.*;

    public class FilterShadow extends Sprite
    {
        protected var s:Sprite;
        public function FilterShadow()
        {
            var tf:TextField = new TextField();
            tf.defaultTextFormat = new TextFormat("_sans", 35, 0, true, false);
            tf.selectable = false;
            tf.width = tf.height = 0;
            tf.autoSize = TextFieldAutoSize.LEFT;
            tf.text = "Hello Shadows";
            s = new Sprite();
            s.addChild(tf);
            tf.x = -tf.textWidth/2;
            tf.y = -tf.textHeight/2;
            s.x = stage.stageWidth/2;
            s.y = stage.stageHeight/2;
            addChild(s);
            s.filters = [new DropShadowFilter(0, 0, 0, 0.4, 12, 12, 1, 2)];
            addEventListener(Event.ENTER_FRAME, onEnterFrame);       
        }

        protected function onEnterFrame(event:Event):void {
            var filter:DropShadowFilter = DropShadowFilter(s.filters[0]);
            var vector:Point = new Point(stage.mouseX - s.x, stage.mouseY - s.y);
            var m:Number = new Point(stage.stageWidth/2, stage.stageHeight/2).length;
            var normalizedDistance:Number = vector.length / m;
            filter.distance = Math.pow(normalizedDistance, 2) * 100;
            filter.blurX = filter.blurY = normalizedDistance * 20;
            filter.alpha = Math.pow(normalizedDistance, 2);
            filter.angle = Math.atan2(vector.y, vector.x) / Math.PI * 180 + 180;
            s.filters = [filter];
        }
    }
}