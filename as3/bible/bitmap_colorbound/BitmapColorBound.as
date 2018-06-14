package
{
    import flash.display.Sprite;
    import flash.display.Loader;
    import flash.events.Event;
    import flash.net.URLRequest;
    import flash.display.Bitmap;
    import flash.display.BitmapData;
    import flash.geom.Point;
    import flash.text.TextField;
    import flash.events.MouseEvent;
    import flash.text.TextFormat;
    import mx.core.BitmapAsset;
    import flash.net.URLLoader;

    public class BitmapColorBound extends Sprite{

        protected var tf:TextField;
        protected var img:Loader;


        public function BitmapColorBound(){

            tf = new TextField();
            tf.width = stage.stageWidth * 0.75;
            tf.height = stage.stageHeight - 4;
            tf.x = 4;
            tf.y = 4;
            tf.defaultTextFormat = new TextFormat("Garamond", 12, 0x303030);
            tf.multiline = tf.wordWrap = true;
            addChild(tf);

            var u:URLLoader  = new URLLoader(new URLRequest("ch8.txt"));
            u.addEventListener(Event.COMPLETE, onTextLoad);

        }

        protected function onTextLoad(evt:Event):void
        {
            tf.text = URLLoader(evt.target).data;
            img = new Loader();
            addChildAt(img, 0);
            img.load(new URLRequest("alice-8.gif"));
            img.contentLoaderInfo.addEventListener(Event.COMPLETE, onImageLoad);
        }

        protected function onImageLoad(evt:Event):void
        {
            img.x = stage.stageWidth - img.width + 30;
            img.y = 30;
            WrapTextUtility.wrapText(tf, img, 2, true);
        }

    }
}

import flash.text.TextField;
import flash.display.DisplayObject;
import flash.geom.Point;
import flash.display.BitmapData;
import flash.text.TextLineMetrics;
import flash.geom.Rectangle;

class WrapTextUtility
{
    private static const KILL_LIMIT:int = 100;
    private static const GUTTER:int = 2;
    private static const NEWLINE:String = "\n";
    private static const WHITESPACE:RegExp = /[\S\-\_]/;

    public static function wrapText(tf:TextField, edge:DisplayObject, paddingPx:int = 6,
        transparentBackground:Boolean = false):void
    {

        if(!tf.hitTestObject(edge))
            return;

        var edgeOffset:Point = edge.localToGlobal(new Point()).subtract(
            tf.localToGlobal(new Point()));
        var lineY:Number = GUTTER;
        var bmpSlice:BitmapData = new BitmapData(edge.width, edge.height, true, 0);

        var i:int = 0;
        do
        {
            try
            {
                var lineMetrics:TextLineMetrics = tf.getLineMetrics(i)
            }
            catch (error:RangeError)
            {
                break;
            }

            var lineBaseline:Number = lineY + lineMetrics.ascent;
            bmpSlice.fillRect(bmpSlice.rect, 0x00000000);
            var clipRect:Rectangle = new Rectangle(0, lineY - edgeOffset.y,
                 edge.width, lineMetrics.ascent + lineMetrics.descent);
            lineY += lineMetrics.height;

            if(clipRect.width <= 0 || clipRect.height <= 0)
                continue;

            bmpSlice.draw(edge, null, null, null, clipRect);

            var colorRect:Rectangle = bmpSlice.getColorBoundsRect(
                (transparentBackground ? 0xFF000000:0xFFFFFFFF),  0x00000000, false);

            if(colorRect == null || colorRect.width <= 0 || colorRect.height <= 0)
                continue;

            var wrapChar:int = tf.getCharIndexAtPoint(colorRect.x + edgeOffset.x - paddingPx, lineBaseline);
            var firstCharInLine:int = tf.getLineOffset(i);
            var allText:String = tf.text;
            if(wrapChar <= 0)
                continue;

            while(allText.charAt(wrapChar).match(WHITESPACE) == null && 
                wrapChar > firstCharInLine)
                --wrapChar;

            if(wrapChar <= firstCharInLine){
                tf.text = allText.slice(0, firstCharInLine) + NEWLINE + allText.slice(firstCharInLine);
            }else{
                tf.text = allText.slice(0, wrapChar) + NEWLINE + allText.slice(wrapChar + 1);
            }
        }
        while(++i < KILL_LIMIT);
        bmpSlice.dispose();
        
        
    }
}