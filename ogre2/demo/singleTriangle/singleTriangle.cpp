#include <CameraController.h>
#include "BaseManager.h"
#include <OgreManualObject2.h>

namespace Demo {

class SingleTriangleGameState : public DemoGameState {
public:
  SingleTriangleGameState() : DemoGameState(
          "a manual triangle\n"
          "if you turn your camera away, this triangle will diapeared because of frustum cull\n"
          ) {}

  virtual void createScene01(void) {
    DemoGameState::createScene01();

    Ogre::SceneManager *mgr = mGraphicsSystem->getSceneManager();
    mManualObject = mgr->createManualObject();

    mManualObject->begin(
        "vertexDiffuse_unlit", Ogre::v1::RenderOperation::OT_TRIANGLE_LIST);

    mManualObject->position(-1, -1, 0.5);
    mManualObject->colour(1, 0, 0, 1);
    mManualObject->position(1, -1, 0.5);
    mManualObject->colour(0, 1, 0, 1);
    mManualObject->position(1, 1, 0.5);
    mManualObject->colour(0, 0, 1, 1);
    mManualObject->triangle(0, 1, 2);

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

class SingleTriangle : public BaseManager {
public:
  SingleTriangle(GameState *gameState) : BaseManager(gameState) {}
};
}

DEFINE_APP(SingleTriangle)
