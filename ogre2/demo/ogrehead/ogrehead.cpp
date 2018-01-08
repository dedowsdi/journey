#include <CameraController.h>
#include "BaseManager.h"
#include "OgreItem.h"
#include "OgreCamera.h"

namespace Demo {

class OgreheadGameState : public DemoGameState {
public:
  OgreheadGameState() : DemoGameState("ogrehead\n") {}

  virtual void createScene01(void) {
    DemoGameState::createScene01();
    Ogre::SceneManager* mgr = mGraphicsSystem->getSceneManager();
    Ogre::Item* item = mgr->createItem("ogrehead.mesh");
    Ogre::SceneNode* node = mgr->getRootSceneNode()->createChildSceneNode();
    node->attachObject(item);

    //light
    Ogre::SceneNode* lnode = mgr->getRootSceneNode()->createChildSceneNode();
    Ogre::Light* light = mgr->createLight();
    light->setType(Ogre::Light::LT_DIRECTIONAL);
    light->setDiffuseColour(1,1,1);
    lnode->attachObject(light);
    light->setDirection(Ogre::Vector3(-1,-1,-1));

    mCameraController = new CameraController(mGraphicsSystem, false);
  }
};

class Ogrehead : public BaseManager {
public:
  Ogrehead(GameState* gameState) : BaseManager(gameState) {}
  virtual void createCamera(void){
      BaseManager::createCamera();
      mCamera->setPosition(Ogre::Vector3(0, 40, 120));
  }
};
}

DEFINE_APP(Ogrehead)
