package com.zz.button
{
    import flash.display.MovieClip;
    import flash.events.MouseEvent;
    import com.zz.Util;

    public class MovieClipScaleButton extends MovieClipButtonBase
    {
        public var rollOverScaleX : Number = 1.1;
        public var rollOverScaleY : Number = 1.1;
        public function MovieClipScaleButton()
        {
            super();
        }

        override protected function onRollOver(evt:MouseEvent):void
        {
            Util.scaleAroundCenter(this, rollOverScaleX, rollOverScaleY);
        }

        override protected function onRollOut(evt:MouseEvent):void
        {
            Util.scaleAroundCenter(this, 1/rollOverScaleX, 1/rollOverScaleY);
        }
    }

}