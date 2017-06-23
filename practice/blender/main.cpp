/*
 * basic blender grab rotate scale
 */
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include "blender.h"
#include "blenderobject.h"
#include <osgDB/ReadFile>
#include "blendermanipulator.h"

int main(int argc, char* argv[]) {
  osg::ArgumentParser ap(&argc, argv);
  ap.getApplicationUsage()->setApplicationName(ap.getApplicationName());
  ap.getApplicationUsage()->setCommandLineUsage(
    ap.getApplicationName() + " [options] filename ...");

  osg::ref_ptr<zxd::Blender> blender = new zxd::Blender();
  osg::ref_ptr<zxd::BlendGuiEventhandler> blenderHandler =
    new zxd::BlendGuiEventhandler();
  blenderHandler->setBlender(blender);

  // load object
  osg::ref_ptr<osg::Node> node = osgDB::readNodeFiles(ap);
  if (!node) node = osgDB::readNodeFile("cessna.osg");

  osg::ref_ptr<zxd::BlenderObject> bo = new zxd::BlenderObject();
  bo->getModel()->addChild(node);

  osg::ref_ptr<osg::Group> root = new osg::Group;

  root->addChild(bo);

  osgViewer::Viewer viewer;
  osg::ref_ptr<osgViewer::StatsHandler> sh = new osgViewer::StatsHandler();
  sh->setKeyEventTogglesOnScreenStats(osgGA::GUIEventAdapter::KEY_I);
  // sh->setKeyEventPrintsOutStats(osgGA::GUIEventAdapter::KEY_J);
  // sh->setKeyEventPrintsOutStats('i');
  viewer.addEventHandler(sh);
  viewer.setSceneData(root);
  osg::ref_ptr<zxd::BlenderManipulator> manipulator =
    new zxd::BlenderManipulator();
  viewer.setCameraManipulator(manipulator);
  osg::Camera* camera = viewer.getCamera();
  // camera->setCullingMode(
  // camera->getCullingMode() & ~osg::CullSettings::SMALL_FEATURE_CULLING);
  camera->setCullingMode(osg::CullSettings::NO_CULLING);
  blender->setCamera(camera);
  blender->setViewer(&viewer);
  root->addChild(blender);

  viewer.addEventHandler(blenderHandler);

  return viewer.run();
}
