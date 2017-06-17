#include <osg/PolygonMode>
#include <osg/MatrixTransform>
#include <osgDB/ReadFile>
#include <osgViewer/Viewer>

int main(int argc, char *argv[]) {

  osg::ref_ptr<osg::Node> model = osgDB::readNodeFile("cessna.osg");

  osg::ref_ptr<osg::MatrixTransform> mt1 = new osg::MatrixTransform;
  mt1->setMatrix(osg::Matrix::translate(-25.0f, 0.0f, 0.0f));
  mt1->addChild(model.get());

  osg::ref_ptr<osg::MatrixTransform> mt2 = new osg::MatrixTransform;
  mt2->setMatrix(osg::Matrix::translate(25.0f, 0.0f, 0.0f));
  mt2->addChild(model.get());

  osg::ref_ptr<osg::PolygonMode> pm = new osg::PolygonMode;
  pm->setMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE);
  mt1->getOrCreateStateSet()->setAttributeAndModes(pm.get());

  osg::ref_ptr<osg::Group> root = new osg::Group;
  root->addChild(mt1.get());
  root->addChild(mt2.get());

  osgViewer::Viewer viewer;
  viewer.setSceneData(root.get());
  return viewer.run();

}
