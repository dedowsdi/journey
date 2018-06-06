// if you want to use another system cursor, use Mouse.cursor
// if you want to use custom cursor, call Mouse.hide(), then show your cursor.
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

    public class CustomCursor extends Sprite{
        protected var customCursor:Loader;

        public function CustomCursor(){
            // load swf as cursor
            customCursor = new Loader();
            var url:String = "http://actionscriptbible.com/files/spinner.swf";
            customCursor.load(new URLRequest(url));
            customCursor.mouseEnabled = false;
            customCursor.mouseChildren = false;
            addChild(customCursor);

            stage.addEventListener(MouseEvent.MOUSE_MOVE, onMouseMove);
            stage.addEventListener(Event.MOUSE_LEAVE, onMouseLeave);
        }

        protected function onMouseMove(evt:MouseEvent):void
        {
            Mouse.hide(); // hide system cursor
            customCursor.visible = true;
            customCursor.x = stage.mouseX;
            customCursor.y = stage.mouseY;
            evt.updateAfterEvent();
        }

        protected function onMouseLeave(evt:Event):void
        {
           customCursor.visible = false; 
        }



    }
}