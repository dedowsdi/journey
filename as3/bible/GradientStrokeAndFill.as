package {
	import flash.display.Sprite;
	import flash.events.MouseEvent;
	import mx.graphics.GradientStroke;

	public class GradientStrokeAndFill extends Sprite {

		public function GradientStrokeAndFill() {
			stage.addEventListener(MouseEvent.CLICK, onClick);
		}

		protected function onClick(event: MouseEvent): void {
			var s: Sticker = new Sticker();
			s.x = stage.mouseX;
			s.y = stage.mouseY;
			s.scaleX = s.scaleY = 0.5 + Math.random() * 0.5;
			s.rotationZ = (Math.random() - 0.5) * 20;
			addChild(s);
		}
	}
}

import flash.display.*;
import flash.text.*;
import flash.filters.*;
import flash.geom.Matrix;

class Sticker extends Sprite {
	protected static const labels: Array = ["OMG!", "WOW!", "BETA", "MFJ", "ORLY",
		"LULZ", "OMFG", "KITTENS", "ZOMG", "WUT?", "2.0", "SNACKS!", "TACO",
		"ONOES!", "DESU"
	];

	protected static const SIZE: Number = 100;

	public function Sticker() {
		var gradientMatrix: Matrix = new Matrix();
		gradientMatrix.createGradientBox(SIZE, SIZE, Math.PI / 2);
		graphics.lineStyle(5, 0, 1, false, null, null, JointStyle.MITER, 10);
		graphics.lineGradientStyle(GradientType.LINEAR, [0xfff600, 0x837749], [1, 1], [0, 255],
			gradientMatrix
		);
		graphics.beginGradientFill(GradientType.LINEAR, [makeColor(), makeColor()], [1, 1], [0, 255],
			gradientMatrix
		);
		drawPolyStar();
		graphics.endFill();
		var label: TextField = makeLabel();
		label.x = -0.5 * label.textWidth;
		label.y = -0.5 * label.textHeight;
		addChild(label);
		filters = [new DropShadowFilter(0, 0, 0, 0.6, 64, 64, 1, 2)];
	}

	protected function drawPolyStar(): void {
		var rdelta: Number = 10; //how pointy the points are
		var tdelta: Number = Math.PI * 2 / 40; //40 points
		for (var t: Number = 0; t < Math.PI * 2; t += tdelta) {
			rdelta *= -1;
			var x: Number = (SIZE + rdelta) * Math.cos(t);
			var y: Number = (SIZE + rdelta) * Math.sin(t);
			if (t == 0) {
				graphics.moveTo(x, y);
			} else {
				graphics.lineTo(x, y);
			}
		}
	}

	protected function makeLabel(): TextField {
		var tf: TextField = new TextField();
		tf.width = tf.height = 0;
		tf.selectable = false;
		tf.autoSize = TextFieldAutoSize.LEFT;
		tf.defaultTextFormat = new TextFormat("_sans", 35, 0xffffff, true, true);
		tf.text = labels[Math.floor(Math.random() * labels.length)];
		return tf;
	}

	protected function makeColor(): uint {
		var rnd: Function = function (): uint {
			return uint(Math.random() * 128 + 128)
		};
		return (rnd() << 16 | rnd() << 8 | rnd());
	}
}