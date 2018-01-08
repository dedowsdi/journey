#include "BaseManager.h"
#include "OgreItem.h"
#include <CameraController.h>
#define MC_TEST 123

namespace Demo {

class SingleCubeGameState : public DemoGameState {
public:
  SingleCubeGameState() : DemoGameState("a single cube") {}

  virtual void createScene01(void) {
    DemoGameState::createScene01();

    int i = MC_TEST;
    std::cout << i;

    SceneManager* mgr = mGraphicsSystem->getSceneManager();

    mManualObject = mgr->createManualObject();

    mManualObject->begin("singleCube", v1::RenderOperation::OT_TRIANGLE_LIST);

    // front face
    mManualObject->position(-1, -1, 1);
    mManualObject->normal(-1, -1, 1);
    mManualObject->colour(ColourValue::Green);
    mManualObject->position(1, -1, 1);
    mManualObject->normal(1, -1, 1);
    mManualObject->colour(ColourValue::Green);
    mManualObject->position(1, 1, 1);
    mManualObject->normal(1, 1, 1);
    mManualObject->colour(ColourValue::Green);
    mManualObject->position(-1, 1, 1);
    mManualObject->normal(-1, 1, 1);
    mManualObject->colour(ColourValue::Green);
    // back face
    mManualObject->position(-1, -1, -1);
    mManualObject->normal(-1, -1, -1);
    mManualObject->colour(ColourValue::Green);
    mManualObject->position(1, -1, -1);
    mManualObject->normal(1, -1, -1);
    mManualObject->colour(ColourValue::Green);
    mManualObject->position(1, 1, -1);
    mManualObject->normal(1, 1, -1);
    mManualObject->colour(ColourValue::Green);
    mManualObject->position(-1, 1, -1);
    mManualObject->normal(-1, 1, -1);
    mManualObject->colour(ColourValue::Green);

    // front
    mManualObject->quad(0, 1, 2, 3);
    // right
    mManualObject->quad(1, 5, 6, 2);
    // top
    mManualObject->quad(3, 2, 6, 7);
    // back
    mManualObject->quad(4, 7, 6, 5);
    // left
    mManualObject->quad(0, 3, 7, 4);
    // bottom
    mManualObject->quad(0, 4, 5, 1);

    mManualObject->end();

    mNode = mgr->getRootSceneNode()->createChildSceneNode();
    mNode->attachObject(mManualObject);

    SceneNode* ln = mgr->getRootSceneNode()->createChildSceneNode();
    Light* l = mgr->createLight();
    ln->attachObject(l);
    l->setDirection(Vector3(-1, -1, -1).normalisedCopy());
    l->setType(Light::LT_DIRECTIONAL);
    l->setDiffuseColour(ColourValue(1, 1, 1, 1));

    mCameraController = new CameraController(mGraphicsSystem, false);
  }

  virtual void update(float timeSinceLast) {
    mNode->yaw(Radian(timeSinceLast * 2));
    mNode->pitch(Radian(timeSinceLast));
    mNode->roll(Radian(timeSinceLast) * 0.5);
    DemoGameState::update(timeSinceLast);
  }

private:
  SceneNode* mNode;
  ManualObject* mManualObject;
};

class SingleCube : public BaseManager {
public:
  SingleCube(GameState* gameState) : BaseManager(gameState) {}
};
}

DEFINE_APP(SingleCube)
