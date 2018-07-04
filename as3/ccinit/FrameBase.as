package
{
    import flash.display.Sprite;
    import flash.events.Event;

    public class FrameBase extends Sprite
    {
        public function FrameBase()
        {
            trace(this.name + " ctor called");
            this.addEventListener(Event.ADDED_TO_STAGE, onAddToStage);
        }

        protected function onAddToStage(evt:Event):void
        {
            trace(this.name + " on added to stage");
        }
    }
}