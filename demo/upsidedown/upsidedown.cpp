#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osg/Geometry>
#include <osgDB/ReadFile>
#include <osg/MatrixTransform>

int main(int argc, char* argv[]) {

  osg::ref_ptr<osg::Node> node = osgDB::readNodeFile("cessna.osg");

  osgViewer::Viewer viewer;
  viewer.addEventHandler(new osgViewer::StatsHandler);
  viewer.setSceneData(node);

  osg::Camera* camera = viewer.getCamera();
  GLdouble left, right, bottom, top, zNear, zfar;
  camera->getProjectionMatrixAsFrustum(left, right, bottom, top, zNear, zfar);
  camera->setProjectionMatrixAsFrustum(left, right, top, bottom, zNear, zfar);
  
  return viewer.run();
}
