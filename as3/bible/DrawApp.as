package {
	import flash.display.Sprite;
	public class DrawApp extends Sprite {
		public function DrawApp() {
			addChild(new DrawingCanvas(stage));
		}
	}
}
import flash.display.*;
import flash.events.*;
import flash.geom.Point;
import flash.ui.Keyboard;

class DrawingCanvas extends Sprite {
	private var canvas: Sprite;
	private var toolState: String = "pen";
	private var curShape: Shape;
	private var curColor: uint;
	private var lineThickness: Number = 4;
	private var colorPicker: ColorPicker;
	private var toolSet: ToolSet;
	private var origin: Point = new Point();

	public function DrawingCanvas(_stage: flash.display.Stage) {

		canvas = new Sprite();
		canvas.graphics.beginFill(0xFFFFFF, 1);
		canvas.graphics.drawRect(0, 0, _stage.stageWidth, _stage.stageHeight);
		canvas.graphics.endFill();
		addChild(canvas);

		_stage.addEventListener(KeyboardEvent.KEY_DOWN, onKeyDown);
		canvas.addEventListener(MouseEvent.MOUSE_DOWN, startDraw);
		canvas.addEventListener(MouseEvent.MOUSE_UP, stopDraw);

		colorPicker = new ColorPicker();
		addChild(colorPicker);
		colorPicker.x = 0;
		colorPicker.addEventListener("colorSelected", changeColor);

		toolSet = new ToolSet();
		addChild(toolSet);
		toolSet.y = 100;
		toolSet.x = 5;
		toolSet.addEventListener("toolSelected", changeTool);
		curShape = new Shape();
	}

	private function onKeyDown(event: KeyboardEvent): void {
		switch (event.keyCode) {
			case Keyboard.UP:
				lineThickness++;
				break;
			case Keyboard.DOWN:
				lineThickness = Math.max(0, lineThickness - 1);
				break;
		}
	}
	private function stopDraw(mouseEvent: MouseEvent): void {
		canvas.removeEventListener(MouseEvent.MOUSE_MOVE, draw);
	}
	private function startDraw(mouseEvent: MouseEvent): void {
		curShape = new Shape();
		origin.x = canvas.mouseX;
		origin.y = canvas.mouseY;
		addChild(curShape);
		canvas.addEventListener(MouseEvent.MOUSE_MOVE, draw);
		if (toolState == "line" || toolState == "pen" || toolState == "eraser") {
			curShape.graphics.moveTo(mouseEvent.stageX, mouseEvent.stageY);
		}
	}
	private function draw(mouseEvent: MouseEvent): void {
		var cursor: Point = new Point(canvas.mouseX, canvas.mouseY);
		var dist: Point = cursor.subtract(origin);
		switch (toolState) {
			case "pen":
				curShape.graphics.lineStyle(lineThickness, curColor);
				curShape.graphics.lineTo(mouseEvent.stageX, mouseEvent.stageY);
				break;
			case "line":
				curShape.graphics.clear();
				curShape.graphics.lineStyle(lineThickness, curColor);
				curShape.graphics.moveTo(origin.x, origin.y);
				curShape.graphics.lineTo(mouseEvent.stageX, mouseEvent.stageY);
				break;
			case "ellipse":
				curShape.graphics.clear();
				curShape.graphics.beginFill(curColor, 1);
				curShape.graphics.drawEllipse(origin.x, origin.y, dist.x, dist.y);
				break;
			case "rect":
				curShape.graphics.clear();
				curShape.graphics.beginFill(curColor, 1);
				curShape.graphics.drawRect(origin.x, origin.y, dist.x, dist.y);
				break;
			case "roundrect":
				curShape.graphics.clear();
				if (dist.x < 0 || dist.y < 0) break;
				curShape.graphics.beginFill(curColor, 1);
				curShape.graphics.drawRoundRect(origin.x, origin.y, dist.x, dist.y,
					lineThickness, lineThickness);
				break;
			case "eraser":
				curShape.graphics.lineStyle(lineThickness, curColor);
				curShape.graphics.lineTo(mouseEvent.stageX, mouseEvent.stageY);
				break;
		}
	}
	private function changeTool(evt: DataEvent): void {
		toolState = evt.data;
		if (toolState == "eraser") {
			curColor = 0xffffff;
			toolState = "pen";
			lineThickness = 30;
		} else {
			lineThickness = 4;
		}
	}
	private function changeColor(dataEvent: DataEvent): void {
		curColor = parseInt(dataEvent.data, 16);
		curShape.graphics.lineStyle(lineThickness, curColor);
		if (toolState == "eraser") {
			toolState = "line";
		}
	}
}
import flash.display.*;
import flash.geom.ColorTransform;
import flash.events.MouseEvent;
import flash.events.DataEvent;

class ColorPicker extends Sprite {
	private var k: int = 255;

	public function ColorPicker() {
		for (var i: int = 0; i < 256; i += 48) {
			for (var j: int = 0; j < 256; j += 48) {

				var spr: Sprite = new Sprite();
				spr.graphics.beginFill(0xFFFFFF, 1);
				spr.graphics.drawRect(0, 0, 10, 10);
				spr.graphics.endFill();
				spr.buttonMode = true;

				//apply color transofrm
				var trans: ColorTransform = new ColorTransform();
				var red: uint = i << 16;
				var green: uint = j << 8;
				var blue: uint = k;

				trans.color = red + green + blue;
				spr.transform.colorTransform = trans;
				addChild(spr);
				spr.addEventListener(MouseEvent.CLICK, onColorPicked);
				spr.x = i / 4;
				spr.y = j / 4;
				k -= 48;
			}
		}
	}
	private function onColorPicked(mouseEvent: MouseEvent): void {
		var target: Sprite = mouseEvent.target as Sprite;
		var trans: ColorTransform = target.transform.colorTransform;
		var color: uint = trans.color;
		var dataEvent: DataEvent = new DataEvent("colorSelected", false, false,
			trans.color.toString(16));
		dispatchEvent(dataEvent);
	}
}
import flash.display.Sprite;
import flash.events.MouseEvent;
import flash.events.DataEvent;
import flash.text.TextField;
import flash.text.TextFieldAutoSize;
class ToolSet extends Sprite {
	public function ToolSet() {

		var pen: Sprite = makeButton("pen");
		pen.graphics.beginFill(0);
		pen.graphics.drawCircle(10, 10, 2);
		pen.graphics.drawCircle(15, 15, 2);
		pen.graphics.drawCircle(20, 20, 2);

		var ellipse: Sprite = makeButton("ellipse");
		ellipse.graphics.beginFill(0)
		ellipse.graphics.drawCircle(15, 15, 12);
		ellipse.graphics.endFill();

		var square: Sprite = makeButton("rect");
		square.graphics.beginFill(0);
		square.graphics.drawRect(5, 5, 20, 20);

		var roundrect: Sprite = makeButton("roundrect");
		roundrect.graphics.beginFill(0);
		roundrect.graphics.drawRoundRect(5, 5, 20, 20, 8);

		var line: Sprite = makeButton("line");
		line.graphics.lineStyle(4);
		line.graphics.moveTo(10, 10);
		line.graphics.lineTo(20, 20);
        
		var eraser: Sprite = makeButton("eraser");
		eraser.graphics.lineStyle(4);
		eraser.graphics.moveTo(10, 10);
		eraser.graphics.lineTo(20, 20);
		eraser.graphics.moveTo(20, 10);
		eraser.graphics.lineTo(10, 20);
	}

	private function makeButton(id: String): Sprite {
		var s: Sprite = new Sprite();
		s.name = id;
		s.graphics.beginFill(0, 0);
		s.graphics.lineStyle(0, 0, 0.3);
		s.graphics.drawRect(0, 0, 30, 30);
		s.graphics.endFill();
		s.graphics.lineStyle();
		s.buttonMode = true;

		s.addEventListener(MouseEvent.MOUSE_DOWN, toolClicked);
		var label: TextField = new TextField();
		label.autoSize = TextFieldAutoSize.LEFT;
		label.text = id;
		label.y = -15;
		s.addChild(label);

		s.y = numChildren * 45;
		addChild(s);
		return s;
	}
	private function toolClicked(event: MouseEvent): void {
		var tool: DisplayObject = event.target as DisplayObject;
		dispatchEvent(new DataEvent("toolSelected", false, false, tool.name));
	}
}