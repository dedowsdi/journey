#ifndef DEMOAPP_H
#define DEMOAPP_H

#include <TutorialGameState.h>
#include "GraphicsSystem.h"
#include "OgreConfigFile.h"
#include "OgreLogManager.h"
#include "OgreRenderWindow.h"
#include "OgreRoot.h"
#include "OgreTimer.h"
#include "SdlInputHandler.h"
#include "Compositor/OgreCompositorManager2.h"

using namespace Ogre;

namespace Demo {

class DemoGameState : public TutorialGameState {
public:
  DemoGameState(const String& desc);
  virtual void keyPressed(const SDL_KeyboardEvent& arg);
};

/*
 *for simplicity, demoapp will derive from both tutorial game state and graphics
 *system
 */
class BaseManager : public GraphicsSystem {
  virtual void setupResources(void);

public:
  BaseManager(GameState* gs);

  void go();
};
}

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define DEFINE_APP(App)                                                      \
  using namespace Demo;                                                      \
  INT WINAPI WinMainApp(HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT) { \
    App##GameState state;                                                    \
    App app(&state);                                                         \
    state._notifyGraphicsSystem(&app);                                       \
    app.initialize(#App);                                                    \
    app.go();                                                                \
  }
#else
#define DEFINE_APP(App)                \
  using namespace Demo;                \
  int main(int argc, char* argv[]) {   \
    App##GameState state;              \
    App app(&state);                   \
    state._notifyGraphicsSystem(&app); \
    app.initialize(#App);              \
    app.go();                          \
  }
#endif
#endif /* DEMOAPP_H */
