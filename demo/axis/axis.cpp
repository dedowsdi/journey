#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osg/Geometry>
#include <osgDB/ReadFile>
#include <osg/MatrixTransform>
#include "auxiliary.h"
#include <osgGA/FlightManipulator>

int main(int argc, char* argv[]) {

  osg::ref_ptr<osg::Group> root = new osg::Group();
  osg::ref_ptr<osg::Node> node = osgDB::readNodeFile("cessna.osg");
  root->addChild(node);

  osg::ref_ptr<zxd::Axes> axes = new zxd::Axes;

  root->addChild(axes);
  
  osgViewer::Viewer viewer;
  viewer.addEventHandler(new osgViewer::StatsHandler);
  viewer.setSceneData(root);

  axes->setCamera(viewer.getCamera());

  return viewer.run();
}
