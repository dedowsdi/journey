// the fill starts at an origin point and floods outward in every direction
// until the color changes
package {
    import flash.display.*;
    import flash.events.MouseEvent;
    import flash.geom.Rectangle;

    public class BitmapFloodFill extends Sprite {
        protected var bmp:BitmapData;

        public function BitmapFloodFill() {
            bmp = new BitmapData(200, 200, false);
            bmp.fillRect(new Rectangle(0, 0, 100, 100), 0xffff0000);
            bmp.fillRect(new Rectangle(100, 0, 100, 100), 0xff00ff00);
            bmp.fillRect(new Rectangle(0, 100, 100, 100), 0xff0000ff);
            bmp.fillRect(new Rectangle(100, 100, 100, 100), 0xffffff00);
            var bitmap:Bitmap = new Bitmap(bmp);
            addChild(bitmap);
            stage.addEventListener(MouseEvent.CLICK, onClick);
        }

        private function onClick(event:MouseEvent):void {
            bmp.floodFill(event.localX, event.localY, Math.random() * 0xffffff);
        }
    }
}