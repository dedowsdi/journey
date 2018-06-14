package
{
    import flash.display.Sprite;

    // weird thing happen if you turn off mouseChildren of root object, the root object itself will stop accept mouse event.
    public class ZZScaleButton extends Sprite{
        public function ZZScaleButton(){
            var btn:RectButton = new RectButton;
            this.addChild(btn);
            btn.x = (stage.stageWidth - btn.width) / 2;
            btn.y = (stage.stageHeight - btn.height) / 2;
        }
    }
}

import com.zz.button.MovieClipScaleButton;
import flash.events.MouseEvent;

class RectButton extends MovieClipScaleButton
{
    public function RectButton()
    {
        this.graphics.beginFill(0x0000ff, 1);
        this.graphics.drawRect(0, 0, 100, 100);
        this.graphics.endFill();
    }

    override protected function onClick(evt:MouseEvent):void{
        trace("123");
    }
}