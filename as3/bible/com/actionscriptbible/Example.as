// The following code is example code from 
// "ActionScript 3.0 Bible, 2nd Edition" by Roger Braunstein
// Find all of the source code on the companion website
// http://actionscriptbible.com/
//
// Copyright (c) 2010 by Wiley Publishing, Inc., Indianapolis, Indiana
//
// Permission is hereby granted, free of charge, to any person
// obtaining a copy of this software and associated documentation
// files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use,
// copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following
// conditions:
//
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
// OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.

package com.actionscriptbible
{
	import flash.display.Sprite;
	import flash.display.StageAlign;
	import flash.display.StageScaleMode;
	import flash.events.Event;
	import flash.system.Capabilities;
	import flash.system.System;
	import flash.text.TextField;
	import flash.text.TextFormat;
	
	public class Example extends Sprite
	{
		protected var _traceTF:TextField;
		public static var globalTrace:Function;
		
		public function Example()
		{
			super();
			Example.globalTrace = this.trace;
			_traceTF = new TextField();
			_traceTF.defaultTextFormat = new TextFormat("_typewriter", 12, 0x0);
			_traceTF.mouseWheelEnabled = true;
      _traceTF.mouseEnabled = false;
      _traceTF.tabEnabled = false;
			_traceTF.multiline = _traceTF.wordWrap = true;
			_traceTF.addEventListener(Event.ADDED_TO_STAGE, function(event:Event):void {
			  stage.scaleMode = StageScaleMode.NO_SCALE;
			  stage.align = StageAlign.TOP_LEFT;
				stage.addEventListener(Event.RESIZE, _example_onResize);
				_example_onResize(null);
			});
			addChild(_traceTF);
			__trace("FP version:", Capabilities.version, "/", Capabilities.playerType, "/", Capabilities.isDebugger?"DEBUG":"PROD");
		}
		
		protected function _example_onResize(event:Event):void
		{
		  _traceTF.width = stage.stageWidth;
		  _traceTF.height = stage.stageHeight;
		}
		protected function trace(...rest):void
		{
			var s:String = rest.join(" ");
			_traceTF.appendText(s + "\n");
			_traceTF.scrollV = _traceTF.maxScrollV;
			__trace(s);
		}
	}
}

var __trace:Function = trace;