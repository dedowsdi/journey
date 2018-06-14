package
{
    import flash.display.Sprite;
    import flash.text.TextField;
    import flash.display.StageScaleMode;
    import flash.display.StageAlign;
    import com.tuto.SimpleTextButton;
    import flash.display.SimpleButton;
    import flash.events.MouseEvent;
    import flash.display.StageDisplayState;
    import flash.events.Event;

    [SWF(width="600", height="600")]
    // [SWF(percentWidth="100", percentHeight='100', backgroundColor='#000000')]
    public class Stage extends Sprite{
        public function Stage(){
            // becareful here. width and height indicates width and height of display list.
            // has no effect ?
            this.stage.stageWidth = 300;
            this.stage.stageHeight = 300;

            var fullscreenButton:SimpleTextButton = new SimpleTextButton("fullscreen");
            fullscreenButton.addEventListener(MouseEvent.CLICK, onFullScreen);
            addChild(fullscreenButton);
        }

        protected function onFullScreen(evt:Event):void
        {
            this.stage.displayState = StageDisplayState.FULL_SCREEN;
        }
    }

}