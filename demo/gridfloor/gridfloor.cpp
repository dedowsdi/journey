#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osg/Geometry>
#include <osgDB/ReadFile>
#include <osg/MatrixTransform>
#include "gridfloor.h"

int main(int argc, char* argv[]) {
  osg::ref_ptr<osg::Group> root = new osg::Group();

  osg::ref_ptr<osg::MatrixTransform> mt0 = new osg::MatrixTransform();
  osg::ref_ptr<osg::MatrixTransform> mt1 = new osg::MatrixTransform();
  osg::ref_ptr<osg::MatrixTransform> mt2 = new osg::MatrixTransform();

  // top
  osg::ref_ptr<zxd::GridFloor> gridFloor0 =
    new zxd::GridFloor(osg::X_AXIS, osg::Y_AXIS);
  gridFloor0->setScale(2.0f);
  gridFloor0->setRowAxisColor(osg::Vec4(osg::X_AXIS, 1));
  gridFloor0->setColAxisColor(osg::Vec4(osg::Y_AXIS, 1));
  gridFloor0->rebuild();
  mt0->addChild(gridFloor0);
  mt0->setMatrix(
    osg::Matrix::translate(osg::Vec3(0.0f, 0, -gridFloor0->getHalfSize())));

  // front
  osg::ref_ptr<zxd::GridFloor> gridFloor1 =
    new zxd::GridFloor(osg::X_AXIS, osg::Z_AXIS);
  gridFloor1->setScale(2.0f);
  gridFloor1->setLines(44);
  gridFloor1->setScale(5);
  gridFloor1->rebuild();
  mt1->addChild(gridFloor1);
  mt1->setMatrix(
    osg::Matrix::translate(osg::Vec3(0.0f, -gridFloor1->getHalfSize(), 0)));

  // right
  osg::ref_ptr<zxd::GridFloor> gridFloor2 =
    new zxd::GridFloor(osg::Y_AXIS, osg::Z_AXIS);
  gridFloor2->setScale(2.0f);
  gridFloor2->rebuild();
  mt2->addChild(gridFloor2);
  mt2->setMatrix(
    osg::Matrix::translate(osg::Vec3(gridFloor2->getHalfSize(), 0.0f, 0)));

  root->addChild(mt0);
  root->addChild(mt1);
  root->addChild(mt2);
  root->addChild(osgDB::readNodeFile("cessna.osg"));

  osgViewer::Viewer viewer;
  viewer.addEventHandler(new osgViewer::StatsHandler);
  viewer.setSceneData(root);

  return viewer.run();
}
