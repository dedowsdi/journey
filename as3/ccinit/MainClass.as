package
{
    import flash.display.Sprite;
    import flash.display.MovieClip;
    import flash.events.KeyboardEvent;
    import flash.ui.Keyboard;
    import flash.events.MouseEvent;

    public class MainClass extends MovieClip
    {
        public var frames:Array=[
            "frame1",
            "frame2",
            "frame3"
        ];
        public function MainClass()
        {
            stop();
            trace("main class init.")

            for(var index:int = 0; index < frames.length; index++)
            {
                this.addFrameScript(index, onMyFrameScript);
            }

            this.stage.addEventListener(KeyboardEvent.KEY_DOWN, onKey);
            this.stage.addEventListener(MouseEvent.CLICK, onClick);
        }

        public function onMyFrameScript():void{
            trace("frame " + this.currentFrame + " script called");
            var index:uint = this.currentFrame - 1;
            var fobjName:String = frames[index];
            var fobj:FrameBase = FrameBase(this.getChildByName(fobjName));

            if(fobj == null)
                trace("empty ") + fobjName;

        }

        protected function onKey(evt:KeyboardEvent):void
        {
            if(evt.keyCode == Keyboard.N){
                this.gotoAndStop((this.currentFrame)%this.frames.length + 1);
            }else if(evt.keyCode == Keyboard.P)
                this.gotoAndStop((this.currentFrame + this.frames.length - 1)%this.frames.length);
        }

        protected function onClick(evt:MouseEvent):void
        {
            trace("click");
        }
    }
}