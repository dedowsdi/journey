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

class MultipleViewportGameState : public DemoGameState {
public:
  MultipleViewportGameState()
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
};

class MultipleViewport : public BaseManager {
public:
  MultipleViewport(GameState* gameState) : BaseManager(gameState) {}
  virtual Ogre::CompositorWorkspace* setupCompositor(void) {
    return mRoot->getCompositorManager2()->addWorkspace(
        mSceneManager, mRenderWindow, mCamera, "viewport2_ws", true);
  }
  virtual void createCamera(void) {
    GraphicsSystem::createCamera();
    Camera* camera2 = mSceneManager->createCamera("camera2");
    camera2->setFixedYawAxis(false);
    camera2->setPosition(0, 25, 0);
    camera2->lookAt(Vector3(0, 0, 0));
    camera2->setNearClipDistance(0.2f);
    camera2->setFarClipDistance(1000.0f);
    camera2->setAutoAspectRatio(true);
  }
};
}

DEFINE_APP(MultipleViewport)
