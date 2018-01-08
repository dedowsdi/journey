#include <BaseManager.h>
#include <CameraController.h>
#include <OgreArchive.h>
#include <OgreCamera.h>
#include <OgreHlms.h>
#include <OgreHlmsDatablock.h>
#include <OgreHlmsManager.h>
#include <OgreViewport.h>
#include <OgreItem.h>
#include <Compositor/OgreCompositorManager2.h>

namespace Demo {

class RTTGameState : public DemoGameState {
public:
  RTTGameState()
      : DemoGameState(
            "a manual triangle\n"
            "if you mv your camera away, this point should diapeared because "
            "of frustum cull\n") {}

  virtual void createScene01(void) {
    DemoGameState::createScene01();

    Ogre::SceneManager* mgr = mGraphicsSystem->getSceneManager();
    Item* item = mgr->createItem("Cube_d.mesh");
    item->setDatablock("singleCube");

    mNode = mgr->getRootSceneNode()->createChildSceneNode();
    mNode->attachObject(item);

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
  CompositorWorkspace* mWorkspace;
};

class RTT : public BaseManager {
public:
  RTT(GameState* gameState) : BaseManager(gameState) {}
};
}

DEFINE_APP(RTT)
