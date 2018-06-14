// Loader.loadBytes() can load ByteArray directly.
package
{
    import flash.display.Sprite;
    import flash.net.URLLoader;
    import flash.net.URLLoaderDataFormat;
    import flash.net.URLRequest;
    import flash.events.Event;
    import flash.display.Loader;
    import flash.utils.ByteArray;

    public class ByteArrayLoad extends Sprite{
        protected var urlLoader:URLLoader;
        public function ByteArrayLoad(){
            urlLoader = new URLLoader();
            urlLoader.dataFormat = URLLoaderDataFormat.BINARY;
            urlLoader.load(new URLRequest("caviar.jpg"));
            urlLoader.addEventListener(Event.COMPLETE, onComplete);
        }

        protected function onComplete(evt:Event):void
        {
            var loader:Loader = new Loader();
            var bytes:ByteArray = urlLoader.data;
            loader.loadBytes(bytes);
            addChild(loader);
        }
    }
}