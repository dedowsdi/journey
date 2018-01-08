#include <CameraController.h>
#include "BaseManager.h"

namespace Demo {

class SinglePointGameState : public DemoGameState {
public:
  SinglePointGameState()
      : DemoGameState(
            "a manual vertex point\n"
            "if you mv your camera away, this point will diapeared because of "
            "frustum cull\n") {}

  virtual void createScene01(void) {
    DemoGameState::createScene01();
    Ogre::SceneManager *mgr = mGraphicsSystem->getSceneManager();
    mManualObject = mgr->createManualObject();

    mManualObject->begin(
        "vertexDiffuse_unlit", Ogre::v1::RenderOperation::OT_POINT_LIST);
    mManualObject->position(0, 0, 0);
    mManualObject->colour(1, 1, 1, 1);
    mManualObject->index(0);

    mManualObject->getSection(0)->setUseIdentityView(true);
    mManualObject->getSection(0)->setUseIdentityProjection(true);

    mManualObject->end();

    Ogre::SceneNode *node = mgr->getRootSceneNode()->createChildSceneNode();
    node->attachObject(mManualObject);

    mCameraController = new CameraController(mGraphicsSystem, false);
  }

private:
  Ogre::ManualObject *mManualObject;
};

class SinglePoint : public BaseManager {
public:
  SinglePoint(GameState *gameState) : BaseManager(gameState) {}
};
}

DEFINE_APP(SinglePoint)
