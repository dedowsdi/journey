package {
    import flash.display.Sprite;
    import flash.events.Event;
    import flash.filters.BevelFilter;
    import flash.filters.DropShadowFilter;
    import flash.geom.Point;
    import flash.text.*;

    public class FilterBevel extends Sprite {
        protected var s:Sprite;

        public function FilterBevel() {
            var tf:TextField = new TextField();
            tf.defaultTextFormat = new TextFormat("_sans", 35, 0x303030, true, false);
            tf.selectable = false;
            tf.width = tf.height = 0;
            tf.autoSize = TextFieldAutoSize.LEFT;
            tf.text = "Hello Bevels";
            s = new Sprite();
            s.addChild(tf);
            tf.x = -tf.textWidth/2;
            tf.y = -tf.textHeight/2;
            s.x = stage.stageWidth/2;
            s.y = stage.stageHeight/2;
            addChild(s);

            s.filters = [new DropShadowFilter(0, 0, 0, 0.4, 12, 12, 1, 2),
            new BevelFilter(2, 0)];

            addEventListener(Event.ENTER_FRAME, onEnterFrame);
        }

        protected function onEnterFrame(event:Event):void {
            var shadow:DropShadowFilter = DropShadowFilter(s.filters[0]);
            var bevel:BevelFilter = BevelFilter(s.filters[1]);
            var vector:Point = new Point(stage.mouseX - s.x, stage.mouseY - s.y);
            var m:Number = new Point(stage.stageWidth/2, stage.stageHeight/2).length;
            var normalizedDistance:Number = vector.length / m;
            shadow.distance = Math.pow(normalizedDistance, 2) * 30;
            shadow.blurX = shadow.blurY = normalizedDistance * 20;
            shadow.alpha = Math.pow(normalizedDistance, 2);
            shadow.angle = Math.atan2(vector.y, vector.x) / Math.PI * 180 + 180;
            bevel.angle = shadow.angle;
            bevel.strength = 2 * normalizedDistance;
            s.filters = [shadow, bevel];
        }
    }
}