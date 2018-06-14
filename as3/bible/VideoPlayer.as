package
{
    import flash.display.Sprite;
    import spark.components.VideoPlayer;
    import flash.media.Video;
    import flash.net.NetConnection;
    import flash.net.NetStream;
    import flash.text.TextField;
    import flash.utils.Timer;
    import flash.events.NetStatusEvent;
    import mx.effects.EffectTargetFilter;
    import flash.events.TimerEvent;
    import flash.events.MouseEvent;
    import flash.text.TextFormat;
    import flash.display3D.textures.TextureBase;
    import flash.display.StageDisplayState;

    public class VideoPlayer extends Sprite{
        protected var video:Video;
        protected var nc:NetConnection;
        protected var ns:NetStream;
        protected var nsDuration:Number;
        protected var playPauseButton:TestButton;
        protected var progressTF:TextField;
        protected var progressTimer:Timer;

        public function VideoPlayer(){
            video = new Video();
            video.x = video.y = 10;
            addChild(video);
            
            nc = new NetConnection;
            nc.connect(null); // local

            ns = new NetStream(nc);
            ns.client= {
                onCuePoin: onCuePoint,
                onMetaData: onMetaData,
                onXMPData: onXMPData,
                onPlayStatus: onPlayStatus
            };

            ns.addEventListener(NetStatusEvent.NET_STATUS, onNetStatus);

            video.attachNetStream(ns);
            ns.play("video.flv");

            playPauseButton = new TestButton(24, 24, "||");
            playPauseButton.x = 10;
            playPauseButton.y = 255;
            playPauseButton.addEventListener(MouseEvent.CLICK, onPlayPauseClicked);
            addChild(playPauseButton);

            var rwButton:TestButton = new TestButton(24, 24, "<<");
            rwButton.x = 39;
            rwButton.y = playPauseButton.y;
            rwButton.addEventListener(MouseEvent.CLICK, onBackClicked);
            addChild(rwButton);

            var fwButton:TestButton = new TestButton(24, 24, ">>");
            fwButton.x = 68;
            fwButton.y = playPauseButton.y;
            fwButton.addEventListener(MouseEvent.CLICK, onForwardClick);
            addChild(fwButton);

            progressTF = new TextField();
            progressTF.x = 97;
            progressTF.y = playPauseButton.y + 4;
            progressTF.width = 140;
            progressTF.height = 24;
            progressTF.defaultTextFormat = 
                new TextFormat("__sans", 11, 0, false, false, false, null, null, "center")
            addChild(progressTF);

            var fsButton:TestButton = new TestButton(82, 24, "fullScreen");
            fsButton.x = playPauseButton.x;
            fsButton.y = playPauseButton.y + 29;
            fsButton.addEventListener(MouseEvent.CLICK, onFullScreenClicked);
            addChild(fsButton);

            progressTimer = new Timer(100);
            progressTimer.addEventListener(TimerEvent.TIMER, onUpdateProgress);
            progressTimer.start();

        }

        protected function onNetStatus(evt:NetStatusEvent):void
        {
            switch(evt.info.code){
                case "Netstream.Play.Stop":
                    ns.seek(0);
                    ns.pause();
                    playPauseButton.label.text = ">";
                    break;
            }
        }

        protected function onPlayStatus(evt:Object):void
        {
            trace(evt);
        }
        
        protected function onCuePoint(evt:Object):void
        {
            progressTF.text = "[cue point] " + evt.name;
            progressTimer.delay = 750;
        }

        protected function onXMPData(evt:Object):void
        {
        
        }

        protected function onMetaData(evt:Object):void
        {
            // get meta data about video here
            nsDuration = evt.duration;
            trace(evt);
            trace("width : ", evt.width);
            trace("height : ", evt.height);
        }

        protected function onUpdateProgress(evt:TimerEvent = null):void
        {
            progressTF.text = ns.time.toFixed(1) + "s / " + nsDuration.toFixed(1) + "s";
            progressTimer.delay = 100;
        }

        protected function onBackClicked(evt:MouseEvent):void
        {
            var seekTime:Number = ns.time - 1;
            seekTime = Math.max(0, seekTime);
            ns.seek(seekTime);
            onUpdateProgress();
        }

        protected function onForwardClick(evt:MouseEvent):void
        {
            var seekTime:Number = ns.time + 1;
            seekTime = Math.min(seekTime, nsDuration);
            ns.seek(seekTime);
            onUpdateProgress();
        }

        protected function onPlayPauseClicked(evt:MouseEvent):void
        {
            if(playPauseButton.label.text == ">"){
                ns.resume();
                playPauseButton.label.text = "||";
            }else{
                ns.pause();
                playPauseButton.label.text = ">";
            }
        }

        protected function onFullScreenClicked(evt:MouseEvent):void
        {
            stage.fullScreenSourceRect = video.getRect(stage);
            stage.displayState = StageDisplayState.FULL_SCREEN;
        }
    }
}

import flash.text.TextField;
import spark.primitives.Graphic;
import flash.display.Sprite;
import flash.text.TextFormat;

class TestButton extends Sprite
{
    public var label:TextField;
    public function TestButton(w:Number, h:Number, labelText:String)
    {
        graphics.lineStyle(0.5, 0, 0, true);
        graphics.beginFill(0xa0a0a0, 1);
        graphics.drawRoundRect(0, 0, w, h, 8);
        graphics.endFill();

        label = new TextField();
        addChild(label);
        label.defaultTextFormat = 
            new TextFormat("__sans", 11, 0, true, false, false, null, null, "center");
        label.width = w;
        label.height = h;
        label.text = labelText;
        label.y = (h-label.textHeight)/2 - 2;
        buttonMode = true;
        mouseChildren = false;
    }
}