package com.zz.button
{
    import flash.display.MovieClip;
    import flash.events.MouseEvent;

    public class MovieClipButtonBase extends MovieClip
    {
        public function MovieClipButtonBase()
        {
            this.buttonMode = true;
            this.mouseChildren = false;
            this.focusRect = false;
            this.tabEnabled = false;
            this.addEventListener(MouseEvent.CLICK, onClick);
            this.addEventListener(MouseEvent.ROLL_OVER, onRollOver);
            this.addEventListener(MouseEvent.ROLL_OUT, onRollOut);
        }

        protected function onClick(evt:MouseEvent):void
        {

        }
        
        protected function onRollOver(evt:MouseEvent):void
        {

        }

        protected function onRollOut(evt:MouseEvent):void
        {

        }
    }
}