package
{
    import flash.display.Sprite;
    import flash.events.MouseEvent;
    import flash.text.TextField;
    import flash.display.DisplayObject;

    public class EventFlow extends Sprite{
        public function EventFlow(){
            var uiContainer:Sprite = new Sprite();
            uiContainer.x = 200;
            uiContainer.name = "uiContainer";
            addChild(uiContainer);
            
            // create the button container and add it to the ui container
            var buttonContainer:Sprite = new Sprite();
            buttonContainer.graphics.beginFill(0x666666);
            buttonContainer.graphics.drawRect(0, 0, 250, 50);
            buttonContainer.name = "buttonContainer";
            buttonContainer.y = 20;
            uiContainer.addChild(buttonContainer);

            // create the ui label and add it to the ui container
            var uiLabel:TextField = new TextField;
            uiLabel.name = "uiLabel";
            uiLabel.text = "Audio Controls";
            uiLabel.selectable = false;
            uiLabel.width = 80;
            uiLabel.height = 20;
            uiContainer.addChild(uiLabel);

            // create three buttons and add them to the button container
            var stopButton:Button = new Button("stop");
            stopButton.x = 10;
            stopButton.y = 10;
            buttonContainer.addChild(stopButton);

            var playButton:Button = new Button("play");
            playButton.x = 90;
            playButton.y = 10;
            buttonContainer.addChild(playButton);

            var pauseButton:Button = new Button("pause");
            pauseButton.x = 170;
            pauseButton.y = 10;
            buttonContainer.addChild(pauseButton);

            // the only play that we add event listener. it should handle
            // everything in bubble phase.
            uiContainer.addEventListener(MouseEvent.CLICK, onClick);
        }

        protected function onClick(evt:MouseEvent):void
        {
            trace("\nClick received.");
            trace("Event Target:", DisplayObject(evt.target).name);
            trace("Current Target:", DisplayObject(evt.currentTarget).name);
        }
    }
}

import flash.display.Sprite;
import flash.text.TextField;

class Button extends Sprite
{
    private var labelField:TextField;

    public function Button(label:String = "button")
    {
        graphics.beginFill(0x3366CC);
        graphics.drawRect(0, 0, 70, 30);
        
        name = label;

        labelField = new TextField();
        labelField.mouseEnabled = false; // no mouse event
        labelField.selectable = false;
        labelField.text = label;
        labelField.x = 10;
        labelField.y = 10;
        labelField.width = 80;
        labelField.height = 20;
        addChild(labelField);
    }
}