package
{
    public class Frame1 extends FrameBase
    {
        public function Frame1()
        {
            trace("frame1 init");
            trace("get farme1 child : obj0 " + this.getChildByName("obj0"));
            trace("get farme1 child : obj1 " + this.getChildByName("obj1"));
            trace("get farme1 child : obj2 " + this.getChildByName("obj2"));
            trace("get farme1 child : obj3 " + this.getChildByName("obj3"));
        }
    }
}