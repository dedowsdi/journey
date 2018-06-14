package
{
    import flash.display.Sprite;
    import fl.controls.TileList;
    import fl.data.DataProvider;
    import fl.controls.ScrollBarDirection;
    import fl.controls.listClasses.ImageCell;
    import fl.controls.ScrollPolicy;
    import flash.events.Event;

    [SWF(width=800, height=600)]
    public class FlTileList extends Sprite{
        private var tl:TileList;
        public var dp:DataProvider;


        public function FlTileList(){
            dp = new DataProvider;

            // var cellStyle:Object = ImageCell.getStyleDefinition();
            // cellStyle.imagePadding = 10;
            // cellStyle.upSkin = BlankRenderer;
            // cellStyle.overSkin = BlankRenderer;

            var len:int = 36
            for(var index:int = 0; index < len; index++)
            {
                // dp.addItem({label:index, source:MyRect, scaleContent:false});
                 dp.addItem({label:index, source:MyRect, iconSrc:MyIcon});
            }

            tl = new TileList;
            tl.dataProvider = dp;
            tl.allowMultipleSelection = false;
            tl.columnWidth = 100;
            tl.rowHeight = 150;
            tl.columnCount = 4;
            tl.rowCount = 2;
            // tl.setSize(tl.columnWidth * 5, tl.rowHeight * 5);
            tl.direction = ScrollBarDirection.VERTICAL;
            tl.setStyle("cellRenderer", BlankRenderer);
            tl.setStyle("skin", Blank); // no outer border
            // tl.setStyle("cellRenderer", BlankRenderer);
            // tl.setStyle("contentPadding", 10); // padding for this while tile list
            // tl.move(10, 10);
            tl.buttonMode = true;
            tl.iconField = "iconSrc";
            addChild(tl);

            tl.addEventListener(Event.CHANGE, onChange);
        }

        protected function onChange(evt:Event):void
        {
            trace(tl.selectedIndex);
        }

    }
}

import flash.display.Sprite;
import fl.controls.listClasses.ICellRenderer;
import fl.controls.listClasses.CellRenderer;
import fl.controls.listClasses.ImageCell;
import fl.controls.ButtonLabelPlacement;

class MyIcon extends Sprite{
    public function MyIcon(){
        this.graphics.beginFill(0x0000ff, 0.5);
        this.graphics.drawCircle(0, 0, 100);
        this.graphics.endFill();
    }

};

class MyRect extends Sprite
{
    public function MyRect(w:uint = 30, h:uint = 30)
    {
        graphics.beginFill(0xff0000, 0.5);
        graphics.drawRect(0, 0, w, h);
        graphics.endFill();
    }
}

class Blank extends Sprite{
    public function Blank(){

    }
}

class BlueRectLine extends Sprite{
    public function BlueRectLine(){
        graphics.lineStyle(0, 0x00ff00, 1, false, "none", null, "round");
        // graphics.lineStyle(0, 0x000000);
        graphics.drawRect(0, 0, 10, 10);
    }
}

class BlackRectLine extends Sprite{
    public function BlackRectLine(){
        graphics.lineStyle(0, 0x000000);
        graphics.drawRect(0, 0, 10, 10);
    }
}

class BlankRenderer extends ImageCell{
    public function BlankRenderer(){
        this.labelPlacement = ButtonLabelPlacement.BOTTOM;
        setStyle("upSkin", Blank);
        setStyle("downSkin", Blank);
        setStyle("overSkin", BlueRectLine);
        setStyle("selectedUpSkin", BlackRectLine);
        setStyle("selectedDownSkin", BlackRectLine);
        setStyle("selectedOverSkin", BlackRectLine);
        setStyle("imagePadding", 5);
        // setStyle("scaleContent", false);
        // setStyle("textPadding", -this.textField.height);
        this.buttonMode = true;
        this.useHandCursor = true;
    }
}