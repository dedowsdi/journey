#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osg/Geometry>
#include <osgDB/ReadFile>
#include <osg/MatrixTransform>
#include <osgViewer/config/SingleWindow>

int main(int argc, char* argv[]) {
   
  osg::ref_ptr<osg::Node> node = osgDB::readNodeFile("cessna.osg");

  osgViewer::Viewer viewer;
  viewer.addEventHandler(new osgViewer::StatsHandler);
  viewer.setSceneData(node);

  //viewer.setUpViewInWindow(50, 50, 800, 600);
  viewer.apply(new osgViewer::SingleWindow(50, 50, 800, 600));

  return viewer.run();
}
