package
{
    import flash.display.Sprite;
    import flash.utils.Timer;

    public class TimerWorldClock extends Sprite{

        public static const TIMER_DELAY:Number = 1 * 1000;

        public function TimerWorldClock(){
            var timer:Timer = new Timer(1000);
            var nyc:LocalClock = new LocalClock(timer, "New Your City, USA", -5);
            var paris:LocalClock = new LocalClock(timer, "paris, France", 1);
            var tokyo:LocalClock = new LocalClock(timer, "Tokyo, Japan", 10);

            addChild(nyc);
            nyc.y = 0;
            addChild(paris);
            paris.y = 50;
            addChild(tokyo);
            tokyo.y = 100;

            timer.start();
        }

    }
}

import flash.display.Sprite;
import flash.text.TextField;
import flash.utils.Timer;
import flash.text.TextFieldAutoSize;
import flash.text.TextFormat;
import flash.events.TimerEvent;
import org.osmf.layout.PaddingLayoutMetadata;

class LocalClock extends Sprite
{
    private var location:String;
    private var timezoneOffset:int;
    private var labelTF:TextField;
    private var clockTF:TextField;

    public function LocalClock(timer:Timer, location:String, tzOffset:int)
    {
        this.location = location;
        this.timezoneOffset = tzOffset;

        labelTF = new TextField();
        clockTF = new TextField();
        labelTF.autoSize = clockTF.autoSize = TextFieldAutoSize.LEFT;
        labelTF.width = labelTF.height = clockTF.width = clockTF.height = 0;
        labelTF.selectable = clockTF.selectable = false;
        labelTF.defaultTextFormat = new TextFormat("__serilf", 12, 0, false, true);
        clockTF.defaultTextFormat = new TextFormat("__typewriter", 12, 0x6AF685);
        clockTF.background = true;
        clockTF.backgroundColor = 0x000000;
        labelTF.text = location;
        clockTF.y = labelTF.textHeight + 5;
        addChild(labelTF);
        addChild(clockTF);

        timer.addEventListener(TimerEvent.TIMER, onTimer);
    }

    protected function onTimer(evt:TimerEvent = null):void
    {
        var date:Date = new Date();
        date.hoursUTC += timezoneOffset;
        clockTF.text = pad(date.hoursUTC) + ":" 
                     + pad(date.minutesUTC) + ":"
                     + pad(date.secondsUTC);
    }

    private function pad(n:Number):String
    {
        var s:String = n.toString();
        while(s.length < 2) s = "0" + s;
        return s;
    }
}