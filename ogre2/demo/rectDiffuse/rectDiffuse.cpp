#include <CameraController.h>
#include "BaseManager.h"
#include <OgreItem.h>
#include <OgreManualObject2.h>
#include <SDL.h>

namespace Demo {

class RectDiffuseGameState : public DemoGameState {
private:
  Ogre::SceneNode *mRectNode;
  Ogre::ManualObject *mManualObject;
  Ogre::StringVector mDatablocks;
  int mDatablockIndex;

public:
  RectDiffuseGameState()
      : DemoGameState(
            "a manual rect with texture\n"
            "press o to switch pbs or unlit\n"),
        mDatablockIndex(0) {}

  virtual void createScene01(void) {
    DemoGameState::createScene01();

    mDatablocks.push_back("rect_diffuse_pbs");
    mDatablocks.push_back("rect_diffuse_unlit");

    Ogre::SceneManager *mgr = mGraphicsSystem->getSceneManager();

    //create a light
    Ogre::Light *light = mgr->createLight();
    light->setType(Ogre::Light::LT_DIRECTIONAL);
    Ogre::SceneNode *lnode = mgr->getRootSceneNode()->createChildSceneNode();
    lnode->attachObject(light);
    light->setDirection(Ogre::Vector3(-1, -1, -1));
    light->setDiffuseColour(1,1,1);

    mRectNode = mgr->getRootSceneNode()->createChildSceneNode();

    mManualObject = mgr->createManualObject();
    mManualObject->begin(mDatablocks[mDatablockIndex],
        Ogre::v1::RenderOperation::OT_TRIANGLE_LIST);
    mManualObject->position(-1, -1, 0.5);
    mManualObject->textureCoord(0, 0);
    mManualObject->normal(0, 0, 1);
    mManualObject->position(1, -1, 0.5);
    mManualObject->textureCoord(1, 0);
    mManualObject->normal(0, 0, 1);
    mManualObject->position(1, 1, 0.5);
    mManualObject->textureCoord(1, 1);
    mManualObject->normal(0, 0, 1);
    mManualObject->position(-1, 1, 0.5);
    mManualObject->textureCoord(0, 1);
    mManualObject->normal(0, 0, 1);
    mManualObject->quad(0, 1, 2, 3);
    // mManualObject->getSection(0)->setUseIdentityView(true);
    // mManualObject->getSection(0)->setUseIdentityProjection(true);
    mManualObject->end();


    Ogre::SceneNode *node = mgr->getRootSceneNode()->createChildSceneNode();
    node->attachObject(mManualObject);

    mCameraController = new CameraController(mGraphicsSystem, false);
  }

  virtual void keyPressed(const SDL_KeyboardEvent &arg) {
    switch (arg.keysym.sym) {
      case SDLK_o:
        mManualObject->setDatablock(0, mDatablocks[++mDatablockIndex % 2]);
        break;
      default:
        break;
    }
    DemoGameState::keyPressed(arg);
  }
};

class RectDiffuse : public BaseManager {
public:
  RectDiffuse(GameState *gameState) : BaseManager(gameState) {}
};
}

DEFINE_APP(RectDiffuse)
