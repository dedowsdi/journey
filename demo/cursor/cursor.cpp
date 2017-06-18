#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osg/Geometry>
#include <osgDB/ReadFile>
#include <osg/MatrixTransform>
#include "auxiliary.h"
#include <osg/io_utils>
#include <osgGA/OrbitManipulator>
#include "common.h"

int main(int argc, char* argv[]) {
  osg::ref_ptr<osg::Group> root = new osg::Group();
  osg::ref_ptr<zxd::Cursor> cursor = new zxd::Cursor();
  root->addChild(cursor);
  osg::ref_ptr<osg::Node> cessna = osgDB::readNodeFile("cessna.osg");
  root->addChild(cessna);

  osg::ref_ptr<zxd::CursorEventHandler> handler = new zxd::CursorEventHandler();

  osgViewer::Viewer viewer;
  viewer.addEventHandler(new osgViewer::StatsHandler);
  viewer.addEventHandler(handler);
  osg::ref_ptr<osgGA::OrbitManipulator> cm = new osgGA::OrbitManipulator();
  viewer.setCameraManipulator(cm);

  handler->setCamera(viewer.getCamera());
  handler->setCursor(cursor);

  viewer.setSceneData(root);
  osg::Camera* camera = viewer.getCamera();
  camera->setCullingMode(
    camera->getCullingMode() & ~osg::CullStack::SMALL_FEATURE_CULLING);

  osg::ref_ptr<osg::Light> light = new osg::Light();
  light->setPosition(osg::Vec4(0, 0, 0, 1.0f));
  light->setDiffuse(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));
  osg::ref_ptr<osg::LightSource> ls = new osg::LightSource();
  ls->setLight(light);

  osg::ref_ptr<osg::MatrixTransform> lmt = new osg::MatrixTransform();
  lmt->setMatrix(osg::Matrix::translate(osg::Vec3(0.0f, 0.0f, 20.0f)));
  lmt->addChild(ls);
  lmt->addChild(new zxd::LightIcon());

  root->addChild(lmt);
  root->addChild(new zxd::Dot());

  osg::Camera* hudCamera = zxd::createHUDCamera();

  osg::ref_ptr<zxd::DirectionArrow> arrow = new zxd::DirectionArrow();
  arrow->setMatrix(osg::Matrix::translate(osg::Vec3(300.0f, 300.0f, 0.0f)));
  hudCamera->addChild(arrow);

  root->addChild(hudCamera);

  root->addChild(new zxd::InfiniteLine());

  return viewer.run();
}
