#include <osg/Switch>
#include <osgDB/ReadFile>
#include <osgViewer/Viewer>

int main(int argc, char *argv[]) {

  osg::ref_ptr<osg::Node> model1 = osgDB::readNodeFile("cessna.osg");
  osg::ref_ptr<osg::Node> model2= osgDB::readNodeFile("cessnafire.osg");

  osg::ref_ptr<osg::Switch> root = new osg::Switch;
  root->addChild( model1.get(), false );
  root->addChild( model2.get(), true );

  osgViewer::Viewer viewer;
  viewer.setSceneData( root.get() );
  return viewer.run();
  
}
