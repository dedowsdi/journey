package
{
    import flash.display.Sprite;

    public class Mask extends Sprite{
        public function Mask(){
            var rect:Sprite = createRect(200);
            var mask:Sprite = createRect(100);

            this.addChild(rect);
            rect.addChild(mask);
            mask.x = mask.y = 100;
            rect.mask = mask;
        }

        protected function createRect(size:Number):Sprite{
            var rect:Sprite = new Sprite;
            rect.graphics.beginFill(0x00ff00, 1);
            rect.graphics.drawRect(0, 0, size, size);
            rect.graphics.endFill();
            return rect;
        }
    }

}