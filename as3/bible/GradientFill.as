package
{
    import flash.display.Sprite;
    import flash.display.GradientType;
    import flash.geom.Matrix;
    import flash.display.SpreadMethod;
    import flash.display.InterpolationMethod;
    import flash.text.TextField;
    import flash.text.TextFieldAutoSize;
    import flash.events.KeyboardEvent;
    import flash.ui.Keyboard;
    import flash.text.TextFormat;

    [SWF(width=600, height=600)]
    public class GradientFill extends Sprite
    {

        public var ratios:Array = [0, 127, 255]; // value / 255 position of gradient box
        public var colors:Array = [0xff0000, 0x00ff00, 0x0000ff];
        public var alphas:Array = [1, 1, 1];

        public var gradientType:String = GradientType.LINEAR;
        public var gradientTypes:Array = [GradientType.LINEAR, GradientType.RADIAL]

        public var spreadMethod:String = SpreadMethod.PAD;
        public var spreadMethods:Array = [SpreadMethod.PAD, SpreadMethod.REFLECT, SpreadMethod.REPEAT]

        public var interpolateMethod:String = InterpolationMethod.RGB;
        public var interpolateMethods:Array = [InterpolationMethod.RGB, InterpolationMethod.LINEAR_RGB]

        public var focalPointRatio:Number = 0;

        public var matrix:Matrix = new Matrix();
        public var boxWidth:Number = 300;
        public var boxHeight:Number = 300;
        public var boxRotatation:Number = 0;
        public var boxTx:Number = 0;
        public var boxTy:Number = 0;
        public var tf:TextField = new TextField;

        public function GradientFill()
        {
            var fmt:TextFormat = new TextFormat();
            fmt.size = 18;

            tf.defaultTextFormat = fmt;
            tf.autoSize = TextFieldAutoSize.LEFT;
            tf.width = 0;
            tf.height = 0;
            tf.multiline = true;

            this.addChild(tf);

            this.stage.addEventListener(KeyboardEvent.KEY_DOWN, onKeyboard);

            redraw();
        }

        public function redraw():void{
            matrix.createGradientBox(boxWidth, boxHeight, boxRotatation, boxTx, boxTy);
            graphics.clear();

            graphics.beginGradientFill(gradientType, colors, alphas, ratios, matrix, spreadMethod, interpolateMethod, focalPointRatio);
            graphics.drawRect(0, 0, this.stage.stageWidth, this.stage.stageHeight);
            graphics.endFill();

            updateText();

        }

        protected function onKeyboard(evt:KeyboardEvent):void
        {
            switch(evt.keyCode){
                case Keyboard.Q:
                    gradientType = nextOption(gradientTypes, gradientType);
                    break;
                
                case Keyboard.W:
                    spreadMethod = nextOption(spreadMethods, spreadMethod);
                    break;

                case Keyboard.E:
                    interpolateMethod = nextOption(interpolateMethods, interpolateMethod);
                    break;

                case Keyboard.R:
                    var oneDegree:Number = Math.PI / 180;
                    boxRotatation += evt.shiftKey ? -oneDegree : oneDegree;
                    break;

                case Keyboard.U:
                    boxWidth += evt.shiftKey ? -1 : 1;
                    break;

                case Keyboard.I:
                    boxHeight += evt.shiftKey ? -1 : 1;
                    break;

                case Keyboard.O:
                    boxTx += evt.shiftKey ? -1 : 1;
                    break;

                case Keyboard.P:
                    boxTy += evt.shiftKey ? -1 : 1;
                    break;
            }

            redraw();
        }

        protected function nextOption(options:Array, option:String):String{
            return options[(options.indexOf(option)+1)%options.length];
        }

        public function updateText():void{
            var s:String;
            s = "q  : gradient type : " + gradientType + "\n"
              + "w  : spread method : " + spreadMethod + "\n"
              + "e  : interpolate method : " + interpolateMethod + "\n"
              + "rR : matrix rotation : " + boxRotatation + "\n"
              + "uR : box width : " + boxWidth + "\n"
              + "iR : box height : " + boxHeight + "\n"
              + "oR : box tx : " + boxTx + "\n"
              + "pR : box ty : " + boxTy + "\n"
            tf.text = s;
        }
    }
}