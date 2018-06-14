// you can only load mp3 to sound
package
{
    import flash.display.Sprite;
    import flash.media.Sound;
    import flash.net.URLRequest;
    import flash.events.Event;
    import flash.media.SoundLoaderContext;
    import flash.media.SoundChannel;
    import com.tuto.SimpleTextButton;
    import flash.events.MouseEvent;
    import flash.media.SoundMixer;

    public class SoundPlay extends Sprite{
        public var song:Sound;
        private var soundChannel:SoundChannel;
        private var isPaused:Boolean = false;
        private var lastPosition:Number = 0;
        private const SEARCH_RATE:int = 2000;

        private var playBtn:SimpleTextButton;
        private var rewindBtn:SimpleTextButton;
        private var stopAllBtn:SimpleTextButton;
        private var pauseBtn:SimpleTextButton;
        private var fastForwardBtn:SimpleTextButton;
        private var fastBackwardBtn:SimpleTextButton;

        public function SoundPlay(){
            song = new Sound();
            song.load(new URLRequest("winter.mp3"), new SoundLoaderContext(5000, true));
            // When you play a sound before it is finished loading, Flash Player
            // automatically buffers the sound, making sure that a certain
            // amount of audio is loaded before actually playing, regardless of
            // when you ask the sound to play.
            onPlay(null);

            var y:int = -40;
            playBtn  = createButton(10, y+=50, "play", onPlay);
            rewindBtn = createButton(10, y+=50, "rewind", onRewind);
            stopAllBtn = createButton(10, y+=50, "stopAll", onStopAll);
            pauseBtn = createButton(10, y+=50, "pause", onPause);
            fastForwardBtn = createButton(10, y+=50, "fastForward", onFastForward);
            fastBackwardBtn = createButton(10, y+=50, "fastBackward", onFastBackward);

        }

        protected function createButton(x:uint, y:uint, text:String, callback:Function):SimpleTextButton{
            var btn:SimpleTextButton = new SimpleTextButton(text);
            btn.x = x;
            btn.y = y;
            btn.addEventListener(MouseEvent.CLICK, callback);
            this.addChild(btn);
            return btn;
        }

        protected function onPlay(evt:Event):void
        {
            if(soundChannel)
                return;

            isPaused = false;
            soundChannel = song.play(lastPosition);
        }

        protected function onRewind(evt:Event):void
        {
            if(!soundChannel)
                return;

            // song.close(); // call this if you want to abort downloading
            soundChannel.stop();
            soundChannel = null;
            lastPosition = 0;
            isPaused = false;
        }

        protected function onStopAll(evt:Event):void
        {
            // you don't have to clean sound channel after you call this.
            SoundMixer.stopAll();
            soundChannel = null;
            lastPosition = 0;
            isPaused = false;
        }

        protected function onPause(evt:Event):void
        {
            if(isPaused)
                return;

            isPaused = true;
            lastPosition = soundChannel.position;
            soundChannel.stop();
            soundChannel = null;
        }

        protected function onFastForward(evt:Event):void
        {
            if(!soundChannel)
                return;

            var pos:Number = soundChannel.position + SEARCH_RATE;
            pos = Math.min(pos, song.length);
            soundChannel.stop();
            soundChannel = song.play(pos);
        }

        protected function onFastBackward(evt:Event):void
        {
            if(!soundChannel)
                return;

            var pos:Number = soundChannel.position - SEARCH_RATE;
            pos = Math.max(pos, 0);
            soundChannel.stop();
            soundChannel = song.play(pos);
        
        }
    }
}
