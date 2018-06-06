// you can not clone anything inherits from DisplayObject?
// you must call registerClassAlias if you want to clone custom 
// object.
package
{
    import flash.display.Sprite;
    import flash.utils.ByteArray;
    import flash.display.Shape;

    public class Clone extends Sprite{
        public function Clone(){
            var obj:Object = {
                a:"a", b:"b"
            };
            var newObj:Object = clone(obj);
            trace(newObj.a, newObj.b);
        }

        public function clone(obj:*):*
        {
            var bytes:ByteArray = new ByteArray();
            bytes.writeObject(obj);
            bytes.position = 0; // rewind

            var newObj:* = bytes.readObject();
            return newObj;
        }
    }

}