package
{
    import flash.display.Sprite;
    import flash.display.SimpleButton;
    import flash.geom.Point;
    import flash.events.MouseEvent;
    import flash.display.DisplayObjectContainer;
    import flash.display.DisplayObject;
    import com.tuto.SimpleTextButton;

    // weird thing happen if you turn off mouseChildren of root object, the root object itself will stop accept mouse event.
    public class MouseChildren extends Sprite{
        public var group:Sprite = new Sprite;
        public function MouseChildren(){
            // this.name = "outside";
            group.name = "middle"
            this.addChild(group);

            var btn0:SimpleTextButton = new SimpleTextButton("button");
            btn0.name = "inner";
            group.addChild(btn0);

            btn0.addEventListener(MouseEvent.CLICK, onClick)
            group.addEventListener(MouseEvent.CLICK, onClick)
            this.addEventListener(MouseEvent.CLICK, onClick);
            this.stage.addEventListener(MouseEvent.CLICK, onClick);

            var btn1:SimpleTextButton = new SimpleTextButton("toggle group childrenMouse");
            btn1.addEventListener(MouseEvent.CLICK, toggleGroupMouseChildren);
            btn1.y = 100;
            group.addChild(btn1);

            var btn2:SimpleTextButton = new SimpleTextButton("toggle root childrenMouse");
            btn2.y = 200;
            btn2.addEventListener(MouseEvent.CLICK, toggleMainMouseChildren);
            group.addChild(btn2);
        }

        public function onClick(evt:MouseEvent):void{
            // this name is root1
            // stage name is null
            trace(evt.currentTarget.name);
        }

        public function toggleMainMouseChildren(evt:MouseEvent):void{
            // if i turn off this.mouseChildren, no one get mouse event, except stage, why?
            this.mouseChildren = !this.mouseChildren;
        }

        public function toggleGroupMouseChildren(evt:MouseEvent):void{
            this.group.mouseChildren = !this.group.mouseChildren;
        }
    }
}