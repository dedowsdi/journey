#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osg/Geometry>
#include <osgDB/ReadFile>
#include <osg/MatrixTransform>
#include "gridfloor.h"

int main(int argc, char* argv[]) {
  osg::ref_ptr<osg::Group> root = new osg::Group();
  osg::ref_ptr<zxd::GridFloor> gridFloor = new zxd::GridFloor();
  gridFloor->setScale(2.0f);
  gridFloor->rebuild();

  root->addChild(gridFloor);

  osgViewer::Viewer viewer;
  viewer.addEventHandler(new osgViewer::StatsHandler);
  viewer.setSceneData(root);

  return viewer.run();
}
