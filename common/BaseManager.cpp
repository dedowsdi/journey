#include "BaseManager.h"

namespace Demo {

//------------------------------------------------------------------------------
DemoGameState::DemoGameState(const String& desc) : TutorialGameState("desc") {}

//------------------------------------------------------------------------------
void DemoGameState::keyPressed(const SDL_KeyboardEvent& arg) {
  switch (arg.keysym.sym) {
    case SDLK_p:
      mGraphicsSystem->getRenderWindow()->writeContentsToTimestampedFile(
          "screen", ".png");
    default:
      break;
  }
  TutorialGameState::keyPressed(arg);
}

//------------------------------------------------------------------------------
void BaseManager::setupResources(void) {
  GraphicsSystem::setupResources();
  Ogre::ConfigFile cf;
  cf.load(mResourcePath + "resources2.cfg");
  Ogre::String dataFolder = cf.getSetting("DoNotUseAsResource", "Hlms", "");
  if (dataFolder.empty())
    dataFolder = "./";
  else if (*(dataFolder.end() - 1) != '/')
    dataFolder += "/";

  dataFolder += "2.0/scripts/materials/PbsMaterials";

  addResourceLocation(dataFolder, "FileSystem", "General");
}

//------------------------------------------------------------------------------
BaseManager::BaseManager(GameState* gs) : GraphicsSystem(gs) {}

//------------------------------------------------------------------------------
void BaseManager::go() {
  static_cast<TutorialGameState*>(mCurrentGameState)
      ->_notifyGraphicsSystem(this);

  this->initialize("a single point");

  if (this->getQuit()) {
    this->deinitialize();
    return;  // User cancelled config
  }

  Ogre::RenderWindow* renderWindow = this->getRenderWindow();

  this->createScene01();
  this->createScene02();

  // Do this after creating the scene for easier the debugging (the mouse
  // doesn't hide itself)
  SdlInputHandler* inputHandler = this->getInputHandler();
  inputHandler->setGrabMousePointer(true);
  inputHandler->setMouseVisible(false);

  Ogre::Timer timer;
  unsigned long startTime = timer.getMicroseconds();

  double timeSinceLast = 1.0 / 60.0;

  while (!this->getQuit()) {
    this->beginFrameParallel();
    this->update(static_cast<float>(timeSinceLast));
    this->finishFrameParallel();
    this->finishFrame();

    if (!renderWindow->isVisible()) {
      // Don't burn CPU cycles unnecessary when we're minimized.
      Ogre::Threads::Sleep(500);
    }

    unsigned long endTime = timer.getMicroseconds();
    timeSinceLast = (endTime - startTime) / 1000000.0;
    timeSinceLast = std::min(1.0, timeSinceLast);  // Prevent from going
                                                   // haywire.
    startTime = endTime;
  }

  this->destroyScene();
  this->deinitialize();

  return;
}
}
