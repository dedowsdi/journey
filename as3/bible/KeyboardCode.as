package
{
    import flash.display.Sprite;
    import flash.display.SimpleButton;
    import flash.geom.Point;
    import flash.events.MouseEvent;
    import flash.display.DisplayObjectContainer;
    import flash.display.DisplayObject;
    import com.tuto.SimpleTextButton;
    import flash.events.KeyboardEvent;

    public class KeyboardCode extends Sprite{
        public var group:Sprite = new Sprite;
        public function KeyboardCode(){
            stage.addEventListener(KeyboardEvent.KEY_DOWN, onKey);
            stage.addEventListener(KeyboardEvent.KEY_UP, onKey);
            trace("event\t\tkeyCode\tcharCode");
        }

        protected function onKey(evt:KeyboardEvent):void
        {
            var char:String = (evt.charCode > 31) ? String.fromCharCode(evt.charCode) : "n/a";
            trace(evt.type + "\t\t" + evt.keyCode + "\t\t" + evt.charCode + "\t(" + char +")");
        }
    }
}