package
{
    import flash.display.Sprite;
    import flash.display.Shape;
    import flash.events.Event;
    import mx.events.SWFBridgeEvent;

    [SWF(backgroundColor="#000000", frameRate="45")]
    public class FillPath extends Sprite{

        protected var radius:Number;
        protected var shape:Shape;

        public function FillPath(){
            shape = new Shape();
            shape.x = stage.stageWidth / 2;
            shape.y = stage.stageHeight / 2;
            addChild(shape);

            radius = Math.min(stage.stageWidth, stage.stageHeight) * 0.3;
            addEventListener(Event.ENTER_FRAME, onEnterFrame);
        }

        protected function onEnterFrame(evt:Event):void
        {
            // deviate circle point with another circle
            // var w:Number =  30 * ((stage.mouseX / stage.stageWidth) - 0.5 );
            var w:Number = int( 30 * ((stage.mouseX / stage.stageWidth) - 0.5 ));
            var radius2:Number = 2 * ((stage.mouseY / stage.stageHeight) - 0.5 );
            radius2 = radius * 3 * Math.pow(radius2, 3);
            shape.graphics.clear();
            shape.graphics.beginFill(0xff00cc, 1);
            for(var t:Number = 0; t < Math.PI* 2; t+= 0.03)
            {
                var x:Number = radius * Math.cos(t) + radius2 * Math.cos(t * w);
                var y:Number = radius * Math.sin(t) + radius2 * Math.sin(t * w);
                if(t == 0)
                    shape.graphics.moveTo(x, y);
                else
                    shape.graphics.lineTo(x, y);
            }
            shape.graphics.endFill();
        }
    }
}