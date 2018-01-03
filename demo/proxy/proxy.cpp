#include <osg/ProxyNode>
#include <osgViewer/Viewer>

int main(int argc, char *argv[])
{
  //load in another thread.
  osg::ref_ptr<osg::ProxyNode> root = new osg::ProxyNode;
  root->setFileName( 0, "cow.osg" );

  osgViewer::Viewer viewer;
  viewer.setSceneData( root.get() );
  return viewer.run();
  
}
