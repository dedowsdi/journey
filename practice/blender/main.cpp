/*
 * basic blender grab rotate scale
 */
#include <osgViewer/CompositeViewer>
#include <osgViewer/ViewerEventHandlers>
#include "blender.h"
#include "blenderobject.h"
#include <osgDB/ReadFile>
#include "blendermanipulator.h"
#include <osg/GraphicsContext>

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

  // create graphics context
  osg::ref_ptr<osg::GraphicsContext::Traits> traits =
    new osg::GraphicsContext::Traits();

  GLuint width = 800, height = 600;
  osg::GraphicsContext::WindowingSystemInterface* wsi =
    osg::GraphicsContext::getWindowingSystemInterface();
  if (!wsi) OSG_FATAL << "failed to get window system interface " << std::endl;
  wsi->getScreenResolution(
    osg::GraphicsContext::ScreenIdentifier(0), width, height);

  traits->x = 0;
  traits->y = 0;
  traits->width = width;
  traits->height = height;
  traits->doubleBuffer = true;
  traits->sharedContext = 0;
  traits->windowDecoration = false;

  osg::GraphicsContext* gc =
    osg::GraphicsContext::createGraphicsContext(traits);
  if (!gc) OSG_FATAL << "failed to creaate graphics context " << std::endl;

  osgViewer::CompositeViewer viewer;
  osg::ref_ptr<osgViewer::View> mainView = new osgViewer::View();
  viewer.addView(mainView);

  osg::ref_ptr<osgViewer::StatsHandler> sh = new osgViewer::StatsHandler();
  sh->setKeyEventTogglesOnScreenStats(osgGA::GUIEventAdapter::KEY_I);
  // sh->setKeyEventPrintsOutStats(osgGA::GUIEventAdapter::KEY_J);
  // sh->setKeyEventPrintsOutStats('i');
  mainView->addEventHandler(sh);
  mainView->setSceneData(root);
  osg::Camera* camera = mainView->getCamera();
  // camera->setCullingMode(
  // camera->getCullingMode() & ~osg::CullSettings::SMALL_FEATURE_CULLING);
  camera->setCullingMode(osg::CullSettings::NO_CULLING);
  camera->setGraphicsContext(gc);
  camera->setViewport(0, 0, width, height);

  blender->setCamera(camera);
  blender->setViewer(&viewer);
  blender->setMainView(mainView);
  blender->createMiniAxes();
  root->addChild(blender);

  osg::ref_ptr<zxd::BlenderManipulator>  camMan = new zxd::BlenderManipulator();
  camMan->setBlender(blender);
  mainView->setCameraManipulator(camMan);

  mainView->addEventHandler(blenderHandler);

  osg::DisplaySettings::instance()->setNumMultiSamples(4);

  //miniview don't need camera mainpulator
  while (!viewer.done()) viewer.frame();


  //return viewer.run();
}
