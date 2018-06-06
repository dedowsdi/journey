package
{
    import flash.display.Sprite;
    import flash.events.Event;

    public class DispatchEvent extends Sprite{
        public function DispatchEvent(){
            var t:Thermometer = new Thermometer();
            t.addEventListener(Thermometer.TEMP_CHANGED, onTempChanged);

            var warn:TempWarning = new TempWarning(t);
            warn.x = stage.stageWidth - warn.width;
            addChild(warn);

            t.debugSimulateCrazyWeather();
        }

        protected function onTempChanged(evt:Event):void
        {
            var t:Thermometer = Thermometer(evt.target);
            trace("It's now " + t.temp.toFixed(1) + "F");
        }
    }
}

import flash.events.EventDispatcher;
import flash.events.Event;
import flash.utils.setInterval;
import flash.display.Sprite;
import flash.geom.ColorTransform;

class TempWarning extends Sprite
{
    public function TempWarning(t:Thermometer)
    {
        graphics.beginFill(0xffff00, 1);
        graphics.drawRect(0, 0, 50, 50);
        graphics.endFill();
        t.addEventListener(Thermometer.TEMP_CHANGED, onTempChanged);
    }

    protected function onTempChanged(evt:Event):void
    {
        var t:Thermometer = Thermometer(evt.target);
        var ctx:ColorTransform = new ColorTransform();
        var hotness:Number = mapRange(t.temp, 60, 100);
        ctx.redMultiplier = hotness;
        ctx.greenMultiplier = 1 - hotness;
        this.transform.colorTransform = ctx;
    }

    // map a range of values from min->max to 0->1 (and clamp)
    protected function mapRange(value:Number, min:Number, max:Number):Number{
        return Math.min(1, Math.max(0, (value-min) / (max - min)));
    }
}

class Thermometer extends EventDispatcher
{
    protected var _temp:Number = 72;
    public static const TEMP_CHANGED:String = "tempChanged";

    public function set temp(newTemp:Number):void
    {
       _temp = newTemp; 
       // dispatch event every time temp changed.
       dispatchEvent(new Event(TEMP_CHANGED));
    }

    public function get temp():Number{
        return _temp;
    }

    internal function debugSimulateCrazyWeather():void{
        // tweak the weather to trend continuously higher (to see our handiwork)
        setInterval(
            function():void {
                // careful here, not _temp
                temp += 3 * (Math.random() - 0.4);
            },
            500
        );
    }

}