package
{
    import flash.display.Sprite;
    import flash.display.Shape;

    public class PixelSnapping extends Sprite{
        public function PixelSnapping(){
            var nosnap:Shape = new Shape();
            addChild(nosnap);
            nosnap.graphics.lineStyle(0, 0, 1, false);
            nosnap.graphics.lineTo(100, 0.4);
            nosnap.graphics.lineTo(100.4, 50);
            nosnap.graphics.lineTo(0, 50.4);
            nosnap.graphics.lineTo(0.4, 0);
            nosnap.x = nosnap.y = 20;

            var snappy:Shape = new Shape();
            addChild(snappy);
            snappy.graphics.lineStyle(0, 0, 1, true);
            snappy.graphics.lineTo(100, 0.4);
            snappy.graphics.lineTo(100.4, 50);
            snappy.graphics.lineTo(0, 50.4);
            snappy.graphics.lineTo(0.4, 0);
            snappy.x = 200;
            snappy.y = 20;
        }
    }
}