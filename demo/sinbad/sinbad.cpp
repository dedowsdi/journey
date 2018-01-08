#include <CameraController.h>
#include "BaseManager.h"
#include <OgreItem.h>
#include <OgreCamera.h>
#include <Animation/OgreTagPoint.h>
#include <Animation/OgreSkeletonInstance.h>

namespace Demo {

class SinbadGameState : public DemoGameState {
public:
  SinbadGameState() : DemoGameState("sinbad\n") {}

  virtual void createScene01(void) {

    //sinbad
    DemoGameState::createScene01();
    Ogre::SceneManager* mgr = mGraphicsSystem->getSceneManager();
    Ogre::Item* sinbad = mgr->createItem("Sinbad.mesh");
    Ogre::SceneNode* node = mgr->getRootSceneNode()->createChildSceneNode();
    node->attachObject(sinbad);

    //sword
    {
        Ogre::Item* sword = mgr->createItem("Sword.mesh");
        Ogre::TagPoint* tp = mgr->createTagPoint();
        tp->attachObject(sword);
        Ogre::Bone* bone =  sinbad->getSkeletonInstance()->getBone("Sheath.L");
        bone->addTagPoint(tp);
    }
    {
        Ogre::Item* sword = mgr->createItem("Sword.mesh");
        Ogre::TagPoint* tp = mgr->createTagPoint();
        tp->attachObject(sword);
        Ogre::Bone* bone =  sinbad->getSkeletonInstance()->getBone("Sheath.R");
        bone->addTagPoint(tp);
    }


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

class Sinbad : public BaseManager {
public:
  Sinbad(GameState* gameState) : BaseManager(gameState) {}
  //virtual void createCamera(void){
      //BaseManager::createCamera();
      //mCamera->setPosition(Ogre::Vector3(0, 10, 30));
  //}
};
}

DEFINE_APP(Sinbad)
