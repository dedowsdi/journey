package
{
    import flash.display.Sprite;
    import flash.display.SimpleButton;
    import flash.geom.Point;
    import flash.events.MouseEvent;
    import flash.display.DisplayObjectContainer;
    import flash.display.DisplayObject;
    import com.tuto.SimpleTextButton;
    import flash.display.Loader;
    import flash.net.URLRequest;
    import flash.ui.Mouse;
    import flash.events.Event;
    import flash.utils.Timer;
    import flash.events.TimerEvent;
    import mx.core.DeferredInstanceFromClass;
    import mx.messaging.AbstractConsumer;
    import flashx.textLayout.formats.WhiteSpaceCollapse;
    import mx.skins.spark.DateChooserNextYearSkin;

    public class TimerBasic extends Sprite{

        public static const TIMER_DELAY:Number = 1 * 1000;
        public var timer:Timer;

        public function TimerBasic(){
            timer = new Timer(TIMER_DELAY, 10);
            timer.addEventListener(TimerEvent.TIMER, onTimer);
            timer.addEventListener(TimerEvent.TIMER_COMPLETE, onTimerComplete);
            timer.start();
        }

        protected function onTimer(evt:TimerEvent):void
        {
            var timer:Timer = Timer(evt.currentTarget);
            var timeElapsed:Number = timer.currentCount * timer.delay;
            var remainCount:Number = timer.repeatCount - timer.currentCount;
            trace("timer elapsed: ", timeElapsed, "ms.")
            if(remainCount > 0)
                trace("there are", remainCount, "ticks remaining");
            trace("tick.");
        }

        protected function onTimerComplete(evt:TimerEvent):void
        {
            trace("ding!");
        }

    }
}