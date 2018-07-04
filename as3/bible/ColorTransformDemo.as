package
{
    import flash.display.Sprite;
    import flash.events.MouseEvent;
    import flash.display.Loader;
    import flash.net.URLRequest;
    import flash.geom.ColorTransform;

    [SWF(width=1024, height=800)]
    public class ColorTransformDemo extends Sprite{
        protected var l:Loader;
        protected var mode:int;

        public function ColorTransformDemo(){
            l = new Loader();
            l.load(new URLRequest("test.jpg"));
            addChild(l);
            l.addEventListener(MouseEvent.MOUSE_MOVE, mouseMoveHandler);
            l.addEventListener(MouseEvent.CLICK, clickHandler);
        }
        protected function mouseMoveHandler(evt:MouseEvent):void
        {
            var value:Number = l.mouseY / l.height;
            // careful, it's just a caopy returned by transform.
            var ctf:flash.geom.ColorTransform = l.transform.colorTransform;
            switch(mode){
                case 0: ctf.redMultiplier = value;
                break;
                case 1: ctf.greenMultiplier = value;
                break;
                case 2: ctf.blueMultiplier = value;
                break;
                case 3: ctf.alphaMultiplier = value;
                break;
            }
            l.transform.colorTransform = ctf;
        }

        protected function clickHandler(evt:MouseEvent):void
        {
            mode = ++mode % 4;
        }

    }
}