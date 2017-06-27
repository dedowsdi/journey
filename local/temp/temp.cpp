#include <iostream>
#include <osg/Matrix>
#include "common.h"
#include <osg/io_utils>
#include <osgViewer/Viewer>
#include <osgGA/StandardManipulator>
#include <ctime>
#include <osgDB/ReadFile>
#include <osgGA/TrackballManipulator>



int main(int argc, char* argv[]) {

  osgViewer::Viewer viewer;
  osg::Camera* camera = new osg::Camera;

  osg::ref_ptr<osg::Node> node = osgDB::readNodeFile("cessna.osg");
  camera->addChild(node);

  camera->setViewMatrixAsLookAt(osg::Vec3(0, -10, 0), osg::Vec3(), osg::Y_AXIS);
  camera->setProjectionMatrixAsPerspective(osg::PI_4, 1.0f, 0.1f, 1000.0f);

  viewer.setSceneData(node);
  viewer.setCameraManipulator(new osgGA::TrackballManipulator);

  while(!viewer.done())
  {
    viewer.frame();
  }
}
