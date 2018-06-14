package
{
    import flash.display.Sprite;
    import flash.display.SimpleButton;
    import flash.geom.Point;
    import flash.events.MouseEvent;
    import flash.display.DisplayObjectContainer;
    import flash.display.DisplayObject;
    import com.tuto.SimpleTextButton;
    import org.osmf.layout.AbsoluteLayoutMetadata;
    import flash.display.Loader;
    import flash.net.URLRequest;
    import flash.events.Event;
    import flash.system.ApplicationDomain;

    // weird thing happen if you turn off mouseChildren of root object, the root object itself will stop accept mouse event.
    public class LoadSwf extends Sprite{
        protected var loader:Loader;
        public function LoadSwf(){
            trace("Loading assets...");
            loader = new Loader();
            loader.load(new URLRequest("asset.swf"));
            loader.contentLoaderInfo.addEventListener(Event.COMPLETE, onLoad);
        }

        protected function onLoad(evt:Event):void
        {
            try
            {
               var assetDomain:ApplicationDomain = loader.contentLoaderInfo.applicationDomain; 
               var MyCircleClass:Class = Class(assetDomain.getDefinition("asset.MyCircle"));
               var circle:DisplayObject = new MyCircleClass;
               this.addChild(circle);
            }
            catch (error:Error)
            {
               trace(error); 
            }
        }
    }
}

import flash.display.Sprite;
import flash.events.MouseEvent;
import mx.events.DragEvent;

internal class DraggableCircle extends Sprite{
    public function DraggableCircle(){
        graphics.beginFill(0, 0.5);
        graphics.drawCircle(0, 0, 50);
        graphics.endFill();
        addEventListener(MouseEvent.MOUSE_DOWN, onStartDrag);
        this.buttonMode = true;
    }

    protected function onStartDrag(evt:MouseEvent):void{
        if(evt && evt.shiftKey){
            cloneAndDrag();
        }else{
            startDrag();
            this.stage.addEventListener(MouseEvent.MOUSE_UP, onStopDrag);
        }
    }

    protected function onStopDrag(evt:MouseEvent):void{
        this.stage.removeEventListener(MouseEvent.MOUSE_UP, onStopDrag);
        stopDrag();
    }

    protected function cloneAndDrag():void{
        var clone:DraggableCircle = new DraggableCircle;
        clone.x = this.x;
        clone.y = this.y;
        this.parent.addChild(clone);
        clone.onStartDrag(null);
    }

}