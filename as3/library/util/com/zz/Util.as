package com.zz{
    import flash.display.DisplayObjectContainer;
    import flash.display.MovieClip;
    import flash.system.Capabilities;
    import flash.display.DisplayObject;
    import flash.text.TextField;
    import flash.text.TextFieldAutoSize;
    import flash.text.TextFormat;
    import flash.system.ApplicationDomain;
    import flash.utils.getQualifiedClassName;
    import avmplus.getQualifiedSuperclassName;
    import flash.display.FrameLabel;
    import flash.utils.describeType;
    import flash.utils.getDefinitionByName;
    import flash.utils.Dictionary;
    import flash.geom.Point;
    import flash.geom.Rectangle;
    import flash.geom.Matrix;

    public class Util{

       static public function traceChildren(ctn:DisplayObjectContainer):void
       {
            if (ctn == null) {
                return;
            }

            trace(ctn.numChildren + " children in " + ctn.name);
            for (var i:uint = 0; i != ctn.numChildren; ++i) {
                if (ctn.getChildAt(i) != null)
                    trace(ctn.getChildAt(i).name);
                else
                    trace(null);
            }   
       }

	    static public function getCurrentSceneIndex(mc: MovieClip): int {
			return getSceneIndex(mc, mc.currentScene.name)
		}

		static public function getSceneIndex(mc:MovieClip, sceneName:String): int {
			for (var i:uint = 0; i != mc.scenes.length; ++i) {
				if (sceneName == mc.scenes[i].name) {
					return i;
				}
			}
			return -1;
		}

        static public function traceVersion(): void {
			var version: String = Capabilities.version;
			var idx: int = version.search(/\d/);
			if (idx != -1) {
				version = version.substr(idx);
				var versions: Array = version.split(',');
				if (versions.length == 4) {
					trace("major version: ", versions[0]);
					trace("minor version: ", versions[1]);
					trace("generated version: ", versions[2]);
					trace("internal generated version: ", versions[3]);
				} else {
					trace('flash player version: ', version);
				}
			} else {
				trace('unknown flash version');
			}
		}

        static public function checkVersion(swf:DisplayObjectContainer, mainVersionNeeded:uint, minorVersionNeeded:uint): void {
			var version: String = Capabilities.version;
			var idx: int = version.search(/\d/);
			if (idx != -1) {
				version = version.substr(idx);
				var versions: Array = version.split(',');
				if (versions.length == 4) {
					var mainVersion:uint = versions[0] as uint;
					var minorVersion:uint = versions[1] as uint;
					if (mainVersion < mainVersionNeeded ||
						(mainVersion == mainVersionNeeded && minorVersion < minorVersionNeeded)) {
						var txt:String = "该swf要求FlashPlayer的版本不低于" + mainVersionNeeded + "." + minorVersionNeeded + "， 您当前的版本为" + mainVersion + "." + minorVersion + "您可前往 https://get.adobe.com/flashplayer/?loc=cn 进行更新";
						var versionText:TextField = new TextField();

						versionText.x = 50;
						versionText.y = 50;
						versionText.autoSize = TextFieldAutoSize.LEFT;
						versionText.text = txt;

						var format: TextFormat = new TextFormat();
						format.size = 30;
						format.color = 0xff0000;
						versionText.setTextFormat(format);

						swf.addChild(versionText);
					}
				}
			} else {
                trace('unknown flash version');
            }
		}

        static public function ifExistsClass(className: String):Boolean {
			if (ApplicationDomain.currentDomain.hasDefinition(className)) {
				return true;
			} else {
				return false;
			}
		}

        // return if class0 inherits from class1.
        // return true for the same class
		public static function ifInherits(class0: Class, class1: Class, domain: ApplicationDomain = null): Boolean {
			if (!class0 || !class1)
                return false;

			var childClassName: String = getQualifiedClassName(class0);

			domain = !domain ? ApplicationDomain.currentDomain : domain;

			while (childClassName != null) {
				var superCls: Class = domain.getDefinition(childClassName) as Class;
				if (superCls == class1)
                    return true;

				childClassName = getQualifiedSuperclassName(superCls);
			}
			return false;
		}


		public static function getFrameLabel(mc: MovieClip, labelName: String): FrameLabel {
            var index:int = getFrameLabelIndex(mc, labelName);
            return index == -1 ? null : mc.currentLabels[index];
		}

		//return -1 if not found
		public static function getFrameLabelIndex(mc: MovieClip, labelName: String): int {
			var frameLabels :Array = mc.currentLabels;
			for (var i:uint = 0; i != frameLabels.length; ++i) {
				if (frameLabels[i].name == labelName)
					return i;
			}

			return -1;
		}

		public static function stopMovieClip(mc:MovieClip, numLayer:uint = 1): void {
			--numLayer;
			for (var i:uint = 0; i != mc.numChildren; ++i) {
				var mcChild : MovieClip = mc.getChildAt(i) as MovieClip;

				if (mcChild) {
					if (numLayer > 0) //destroy child
						stopMovieClip(mcChild, numLayer);
				}
			}

			mc.stop();
		}

		public static function hideChildren(ctn: DisplayObjectContainer): void {
			for (var i:uint = 0; i != ctn.numChildren; ++i) {
				var child:DisplayObject = ctn.getChildAt(i);
				child.visible = false;
			}
		}

		public static function getChildIndexByName(ctn: DisplayObjectContainer, strName: String): int {
			return ctn.getChildIndex(ctn.getChildByName(strName));
		}

		public static function clearChildren(ctn: DisplayObjectContainer) : void {
			while (ctn.numChildren > 0)
				ctn.removeChildAt(0);
		}

		public static function dictLength(dict: Dictionary): int {
			var i:int = 0;
			for (var key: Object in dict) {
				++i;
			}
			return i;
		}

		public static function arraySetTo(arr: Array, v:*) : void {
			for (var i: int = 0; i < arr.length; i++) {
				arr[i] = v;
			}
		}

		public static function arrayPush(arr:Array, v:*, num: int): void {
            while(num--)
				arr.push(v);
		}

		public static function arrayUnique(arr: Array, ifNeedSort:Boolean = true): Array {
			if (arr.length == 0)
				return null;

			if (ifNeedSort)
				arr.sort();

			var arr2: Array = new Array();
			arr2.push(arr[0]);
			var index2:int = 0;

			for (var index1: int = 1; index1 < arr.length; index1++) {
				if (arr2[index2] != arr[index1])
				{
					++index2;
					arr2.push(arr[index1]);
				}
			}
			return arr2;
		}

		public static function getParentUntil(mc: DisplayObject, parentType: Class): Object {
			if (mc is parentType) {
				return mc as parentType;
			} else if (mc.parent && mc.parent is DisplayObject) {
				return getParentUntil(mc.parent, parentType);
			} else {
				return null;
			}
		}

		/**
		 * Scale around an arbitrary centre point
		 * @param Number local horizontal offset from 'real' registration point
		 * @param Number local vertical offset from 'real' registration point
		 * @param Number relative scaleX increase; e.g. 2 to double, 0.5 to half
		 * @param Number relative scaleY increase
		 *
		 * B as register center, A as origin, C as scale center
		 * AC will not change
		 * CB will be scaled
		 * than AB = AC + CB
         * all calculation done in parent frame
		 */
		protected static function scaleAround(obj: DisplayObject, offsetX: Number, offsetY: Number, relScaleX: Number, relScaleY: Number): void {

			// map vector to centre point within parent scope 
			var AC: Point = new Point(offsetX, offsetY);
			AC = obj.localToGlobal(AC);
			AC = obj.parent.globalToLocal(AC);
			// current registered postion AB 
			var AB: Point = new Point(obj.x, obj.y);
			// CB = AB - AC, obj vector that will scale as it runs from the centre 
			var CB: Point = AB.subtract(AC);
			CB.x *= relScaleX;
			CB.y *= relScaleY;
			// recaulate AB, obj will be the adjusted position for the clip 
			AB = AC.add(CB);
			// set actual properties 
			obj.scaleX *= relScaleX;
			obj.scaleY *= relScaleY;
			obj.x = AB.x;
			obj.y = AB.y;
		}

		public static function scaleAroundCenter(obj: DisplayObject, relScaleX: Number, relScaleY: Number) : void {
			var offset: Point = getBoundCenter(obj, obj);
			scaleAround(obj, offset.x, offset.y, relScaleX, relScaleY);
		}

		public static function getBoundCenter(obj: DisplayObject, targetCoordinateSpace: DisplayObject): Point {
			var bound: Rectangle = obj.getBounds(targetCoordinateSpace);
			return new Point(bound.x + bound.width * 0.5, bound.y + bound.height * 0.5);
		}

		/**
		 * Rotate around an arbitrary centre point
		 * @param offsetX Number local horizontal offset from 'real' registration point
		 * @param offsetY Number local vertical offset from 'real' registration point
		 * @param toDegrees Number absolute rotation in degrees
		 */
		public static function rotateAround(obj: DisplayObject, offsetX: Number, offsetY: Number, degrees: Number): void {
			var absDegrees: Number = (degrees + obj.rotation) % 360;
			var absRadians: Number = Math.PI * absDegrees / 180;
			var M: Matrix = new Matrix();
			M.rotate(absRadians);
			// map vector to centre point within parent scope 
			var AC: Point = new Point(offsetX, offsetY);
			AC = obj.localToGlobal(AC);
			AC = obj.parent.globalToLocal(AC);
			// current registered postion AB 
			var AB: Point = new Point(obj.x, obj.y);
			// point to rotate, offset position from virtual centre 
			var CB: Point = AB.subtract(AC);
			// rotate CB around imaginary centre  
			// then get new AB = AC + CB 
			CB = M.transformPoint(CB);
			AB = AC.add(CB);
			// set real values on clip 
			obj.rotation = absDegrees;
			obj.x = AB.x;
			obj.y = AB.y;
		}


        private static function newSibling(sourceObj:Object):* {
                if(sourceObj) {
    
                var objSibling:*;
                try {
                    var classOfSourceObj:Class = getDefinitionByName(getQualifiedClassName(sourceObj)) as Class;
                    objSibling = new classOfSourceObj();
                }
    
                catch(e:Object) {
                    trace("failed to create sibling");
                }
    
                return objSibling;
            }
            return null;
        }
    
        public static function clone(source:Object):Object {
    
            var clone:Object;
            if(source) {
                clone = newSibling(source);

                if(clone) {
                    copyData(source, clone);
                }
            }

            return clone;
        }
    
        private static function copyData(source:Object, destination:Object):void {
    
            //copies data from commonly named properties and getter/setter pairs
            if((source) && (destination)) {
    
                try {
                    var sourceInfo:XML = describeType(source);
                    var prop:XML;

                    for each(prop in sourceInfo.variable) {

                        if(destination.hasOwnProperty(prop.@name)) {
                            destination[prop.@name] = source[prop.@name];
                        }

                    }

                    for each(prop in sourceInfo.accessor) {
                        if(prop.@access == "readwrite") {
                            if(destination.hasOwnProperty(prop.@name)) {
                                destination[prop.@name] = source[prop.@name];
                            }

                        }
                    }
                }
                catch (err:Object) {
                    trace("failed to copy data");
                }
            }
        }

    }
}