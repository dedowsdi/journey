package
{
    import flash.display.Sprite;
    import flash.text.TextFormat;
    import flash.text.TextField;
    import flash.text.TextFieldAutoSize;

    public class TextFieldAuto extends Sprite{
        public function TextFieldAuto(){
            var format:TextFormat = new TextFormat;
            format.size = 30;


            var tf:TextField = new TextField;
            tf.defaultTextFormat = format;
            tf.autoSize = TextFieldAutoSize.LEFT;
            trace(tf.width); // 100
            tf.width = 0;
            trace(tf.width); // 4
            tf.text = "bbbbbbbbbbaaaaaaaaaa";
            trace(tf.width); // 284
            trace(tf.textWidth); // 284

            this.addChild(tf);
            tf.x = (this.stage.stageWidth - tf.width)/2;
            tf.y = (this.stage.stageHeight - tf.height)/2;
        }
    }
}