#include <BaseManager.h>
#include <CameraController.h>
#include <OgreArchive.h>
#include <OgreCamera.h>
#include <OgreHlms.h>
#include <OgreHlmsDatablock.h>
#include <OgreHlmsManager.h>
#include <OgreViewport.h>
#include <Compositor/OgreCompositorManager2.h>

namespace Demo {

class ScissorGameState : public DemoGameState {
public:
  ScissorGameState()
      : DemoGameState(
            "a manual triangle\n"
            "if you mv your camera away, this point should diapeared because "
            "of frustum cull\n") {}

  virtual void createScene01(void) {
    DemoGameState::createScene01();

    Ogre::SceneManager *mgr = mGraphicsSystem->getSceneManager();

    Hlms *hlms = Root::getSingleton().getHlmsManager()->getHlms(HLMS_UNLIT);

    HlmsMacroblock mb;
    mb.mScissorTestEnabled = true;

    hlms->createDatablock(
        "scissor", "scissor", mb, HlmsBlendblock(), HlmsParamVec());

    // create a triangle
    mManualObject = mgr->createManualObject();

    mManualObject->begin(
        "scissor", Ogre::v1::RenderOperation::OT_TRIANGLE_LIST);

    mManualObject->position(-5, -5, 0.5);
    mManualObject->colour(1, 0, 0, 1);
    mManualObject->position(5, -5, 0.5);
    mManualObject->colour(0, 1, 0, 1);
    mManualObject->position(5, 5, 0.5);
    mManualObject->colour(0, 0, 1, 1);
    mManualObject->triangle(0, 1, 2);

    mManualObject->end();

    Ogre::SceneNode *node = mgr->getRootSceneNode()->createChildSceneNode();
    node->attachObject(mManualObject);

    mCameraController = new CameraController(mGraphicsSystem, false);
  }

private:
  Ogre::ManualObject *mManualObject;
};

class Scissor : public BaseManager {
public:
  Scissor(GameState *gameState) : BaseManager(gameState) {}
  virtual Ogre::CompositorWorkspace *setupCompositor(void) {
    return mRoot->getCompositorManager2()->addWorkspace(
        mSceneManager, mRenderWindow, mCamera, "scissor_ws", true);
  }
};
}

DEFINE_APP(Scissor)
