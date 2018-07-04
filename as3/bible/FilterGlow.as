package
{
    import flash.display.Sprite;

    [SWF(backgroundColor="#000000")]
    public class FilterGlow extends Sprite
    {
        public function FilterGlow()
        {
            var game:RepeatGame = new RepeatGame(stage);
            addChild(game);
            game.x = stage.stageWidth/2;
            game.y = stage.stageHeight/2;
            game.startGame();
        }
    }
}

import flash.filters.GlowFilter;
import flash.display.*;
import flash.events.*;
import flash.utils.*;
import flash.ui.Keyboard;
import flash.media.Sound;

class RepeatGame extends Sprite {

    protected var cpuMoves:Array;
    protected var moveIndex:int;
    protected var isCpuTurn:Boolean;
    protected var movePlaybackTimer:Timer;
    protected var piecesByKeyCode:Array;
    protected var piecesById:Array;

    public function RepeatGame(s:flash.display.Stage) {
        s.addEventListener(KeyboardEvent.KEY_DOWN, onPlayerMove);
        movePlaybackTimer = new Timer(0);
        movePlaybackTimer.addEventListener(TimerEvent.TIMER, onCpuMove);
        buildPieces();
    }

    protected function buildPieces():void {
        piecesById = new Array();
        piecesByKeyCode = new Array();
        var piece:GamePiece;
        var arci:Number = - 3/4 * Math.PI;
        piece = new GamePiece(0x47cf51, 0x00ff00, arci, arci + Math.PI/2, 220);
        piecesByKeyCode[Keyboard.UP] = piece;
        piecesById.push(piece);
        addChild(piece);
        arci += Math.PI/2;
        piece = new GamePiece(0xe52a37, 0xff0000, arci, arci + Math.PI/2, 293);
        piecesByKeyCode[Keyboard.RIGHT] = piece;
        piecesById.push(piece);
        addChild(piece);
        arci += Math.PI/2;
        piece = new GamePiece(0x3e49da, 0x0000ff, arci, arci + Math.PI/2, 392);
        piecesByKeyCode[Keyboard.DOWN] = piece;
        piecesById.push(piece);
        addChild(piece);
        arci += Math.PI/2;
        piece = new GamePiece(0xf5ed10, 0xffff00, arci, arci + Math.PI/2, 440);
        piecesById.push(piece);
        piecesByKeyCode[Keyboard.LEFT] = piece;
        addChild(piece);
    }
    public function startGame():void {
        movePlaybackTimer.delay = 2000;
        cpuMoves = new Array();
        nextRound();
    }
    protected function nextRound():void {
        trace("NEXT ROUND!");
        isCpuTurn = true
        cpuMoves.push(int(Math.random() * piecesById.length));
        moveIndex = 0;
        movePlaybackTimer.delay *= 0.75;
        movePlaybackTimer.reset();
        movePlaybackTimer.start();
    }

    protected function onCpuMove(event:TimerEvent):void {
        GamePiece(piecesById[cpuMoves[moveIndex]]).flash();
        if (++moveIndex >= cpuMoves.length) {
            isCpuTurn = false;
            moveIndex = 0;
            movePlaybackTimer.stop();
            return;
        }
    }
    protected function onPlayerMove(event:KeyboardEvent):void {
        var selectedPiece:GamePiece = piecesByKeyCode[event.keyCode] as GamePiece;
        if (selectedPiece == null || isCpuTurn) return;
        selectedPiece.flash();
        var correct:Boolean = (selectedPiece == piecesById[cpuMoves[moveIndex]]);
        if (correct) {
            trace("RIGHT!");
            if (++moveIndex >= cpuMoves.length) {
                isCpuTurn = true;
                setTimeout(nextRound, movePlaybackTimer.delay);
            }
        } else {
            trace("WRONG!");
            isCpuTurn = true;
            setTimeout(startGame, 1000);
        }
    }
}

class GamePiece extends Sprite {

    protected const INNER_RADIUS:Number = 100;
    protected const OUTER_RADIUS:Number = 160;
    protected const ARC_PADDING:Number = 0.06;
    protected var glow:GlowFilter;
    protected var pitchHz:Number;

    public function GamePiece(color:uint, glowColor:uint, arcStartAngle:Number, arcEndAngle:Number, pitchHz:Number) {
        this.pitchHz = pitchHz;
        arcStartAngle += ARC_PADDING;
        arcEndAngle -= ARC_PADDING;
        graphics.beginFill(color);
        var step:Number = (arcEndAngle - arcStartAngle) / 200;
        var theta:Number;

        for (theta = arcStartAngle; theta <= arcEndAngle; theta += step) {
            var X:Number = Math.cos(theta) * INNER_RADIUS;
            var Y:Number = Math.sin(theta) * INNER_RADIUS;
            if (theta == arcStartAngle) {
                graphics.moveTo(X, Y);
            } else {
                graphics.lineTo(X, Y);
            }
        }

        for (theta = arcEndAngle; theta >= arcStartAngle; theta -= step) {
            graphics.lineTo(Math.cos(theta) * OUTER_RADIUS,
            Math.sin(theta) * OUTER_RADIUS);
        }

        graphics.endFill();

        glow = new GlowFilter(glowColor, 0, 32, 32, 2, 2, false);
        addEventListener(Event.ENTER_FRAME, onEnterFrame);
    }

    public function flash():void {
        glow.alpha = 1;
        var s:Sound = new Sound();
        s.addEventListener(SampleDataEvent.SAMPLE_DATA, onBufferSound);
        s.play();
    }

    protected function onBufferSound(event:SampleDataEvent):void {
        var tEnd:int = 44100 * 0.5; //duration = 0.5sec
        var tMax:int = Math.min(tEnd, event.position + 8192);
        for (var t:int = event.position; t < tMax; t++) {
            var n:Number = Math.sin(t / 44100 * 2 * Math.PI * pitchHz);
            n *= ((tEnd-t) / tEnd) * 0.5; //fade out
            event.data.writeFloat(n);
            event.data.writeFloat(n);
        }
    }
    protected function onEnterFrame(event:Event):void {
        if (glow.alpha > 0) {
            glow.alpha -= 0.05;
            filters = [glow];
        } else {
            filters = null;
        }
    }
}