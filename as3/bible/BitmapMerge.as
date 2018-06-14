package
{
    import flash.display.Sprite;
    import flash.display.BitmapData;
    import flash.geom.Rectangle;
    import flash.geom.Point;
    import flash.display.Bitmap;

    public class BitmapMerge extends Sprite{
        public function BitmapMerge(){
            var sizeRect:Rectangle = new Rectangle(0, 0, 50, 50);
            var bda:BitmapData = new BitmapData(sizeRect.width, sizeRect.height);
            bda.fillRect(sizeRect, 0xffff0000);

            var bdb:BitmapData = new BitmapData(sizeRect.width, sizeRect.height);
            bdb.fillRect(sizeRect, 0xff0000ff);

            var steps:int = 60;
            var wrapAt:Number = stage.stageWidth;
            var destination:Point = new Point();
            var origin:Point = new Point();
            var bd:BitmapData = new BitmapData(stage.stageWidth, stage.stageHeight);
            var bdTemp:BitmapData = new BitmapData(sizeRect.width, sizeRect.height);

            for(var i:int = 0; i < steps; i++)
            {
                // clear temp bitma
                bdTemp.fillRect(sizeRect, 0);
                bdTemp.copyPixels(bda, bda.rect, origin);
                var blend:int = i / steps * 256;
                // blend  B into temp
                bdTemp.merge(bdb, bdb.rect, origin, blend, blend, blend, blend);
                bd.copyPixels(bdTemp, bdTemp.rect, destination);

                destination.x += sizeRect.width;
                if(destination.x + sizeRect.width > wrapAt)
                {
                    destination.y += sizeRect.height;
                    destination.x = 0;
                }
            }

            var bitmap:Bitmap = new Bitmap(bd);
            this.addChild(bitmap);
        }
    }
}