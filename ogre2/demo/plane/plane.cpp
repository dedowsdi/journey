#include <CameraController.h>
#include "BaseManager.h"
#include <OgreMeshManager.h>
#include <OgreMeshManager2.h>
#include <OgreMesh2.h>
#include <OgreItem.h>

namespace Demo {

class DemoPlaneGameState : public DemoGameState {
public:
  DemoPlaneGameState() : DemoGameState("a plane\n") {}

  virtual void createScene01(void) {
    DemoGameState::createScene01();

    SceneManager *mgr = mGraphicsSystem->getSceneManager();

    v1::MeshPtr planeMeshV1 = v1::MeshManager::getSingleton().createPlane(
        "DemoPlane v1", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
        Plane(Vector3::UNIT_Y, 1.0f), 50.0f, 50.0f, 1, 1, true, 1, 5.0f, 5.0f,
        Vector3::UNIT_Z, v1::HardwareBuffer::HBU_STATIC,
        v1::HardwareBuffer::HBU_STATIC);

    MeshPtr planeMesh = MeshManager::getSingleton().createManual(
        "DemoPlane", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

    planeMesh->importV1(planeMeshV1.get(), true, true, true);

    Item *item = mgr->createItem(planeMesh, SCENE_DYNAMIC);
    item->setDatablock("Marble");
    SceneNode *sceneNode = mgr->getRootSceneNode(SCENE_DYNAMIC)
                               ->createChildSceneNode(SCENE_DYNAMIC);
    sceneNode->setPosition(0, -1, 0);
    sceneNode->attachObject(item);

    SceneNode *ln = mgr->getRootSceneNode(SCENE_DYNAMIC)->createChildSceneNode();

    Light* light = mgr->createLight();
    ln->attachObject(light);
    light->setType(Light::LT_DIRECTIONAL);
    light->setDirection(Vector3(-1,-1,-1));

    mCameraController = new CameraController(mGraphicsSystem, false);


  }

};

class DemoPlane : public BaseManager {
public:
  DemoPlane(GameState *gameState) : BaseManager(gameState) {}
};
}

DEFINE_APP(DemoPlane)
