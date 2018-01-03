#include <osg/MatrixTransform>
#include <osgDB/ReadFile>
#include <osgViewer/Viewer>


int main(int argc, char *argv[]) {

  osg::ref_ptr<osg::Node> model = osgDB::readNodeFile("cessna.osg");

  osg::ref_ptr<osg::MatrixTransform> transform1 = new osg::MatrixTransform;
  transform1->setMatrix(osg::Matrix::translate(-25.0f, 0.0f, 0.0f));
  transform1->addChild(model.get());

  //a node can have multiple parents
  osg::ref_ptr<osg::MatrixTransform> transform2 = new osg::MatrixTransform;
  transform2->setMatrix(osg::Matrix::translate(25.0f, 0.0f, 0.0f));
  transform2->addChild(model.get());

  osg::ref_ptr<osg::Group> root = new osg::Group;
  root->addChild(transform1.get());
  root->addChild(transform2.get());

  osgViewer::Viewer viewer;
  viewer.setSceneData(root.get());
  return viewer.run();

  return 0;
}
