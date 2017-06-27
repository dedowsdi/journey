#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osg/Geometry>
#include <osgDB/ReadFile>
#include <osg/MatrixTransform>
#include "auxiliary.h"
#include <osgGA/FlightManipulator>
#include <osg/AutoTransform>
#include "common.h"

int main(int argc, char* argv[]) {
  osg::ref_ptr<osg::Group> root = new osg::Group();

  osg::Camera* camera = zxd::createHUDCamera();
  //camera->setCullingActive(false);
  camera->setCullingMode(camera->getCullingMode() & ~osg::CullSettings::SMALL_FEATURE_CULLING);

  osg::ref_ptr<zxd::Axes> axes = new zxd::Axes;
  axes->setLabel(true);
  axes->setMatrix(osg::Matrix::scale(200.0f, 200.0f, 200.0f) *
                  osg::Matrix::rotate(-osg::PI_4, osg::X_AXIS) *
                  osg::Matrix::translate(200.0f, 200.0f, 0));

  root->addChild(camera);
  camera->addChild(axes);
  camera->setCullingActive(false);
  osg::ref_ptr<osg::Node> cessna = osgDB::readNodeFile("cessna.osg");
  osg::ref_ptr<osg::MatrixTransform> mt = new osg::MatrixTransform();
  mt->setMatrix(osg::Matrix::scale(10.0f, 10.0f, 10.0f ) * osg::Matrix::translate(osg::Vec3(500.0f, 500.0f, 0)));
  mt->addChild(cessna);
  camera->addChild(mt);

  osgViewer::Viewer viewer;
  viewer.addEventHandler(new osgViewer::StatsHandler);
  viewer.setSceneData(root);

  return viewer.run();
}
