#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osg/Geometry>
#include <osgDB/ReadFile>
#include <osg/MatrixTransform>

/*
 * light attribute is not global
 */
int main(int argc, char* argv[]) {

  osg::ref_ptr<osg::Node> model = osgDB::readNodeFile("cessna.osg");
  
  osg::ref_ptr<osg::MatrixTransform> mt0  = new osg::MatrixTransform();
  osg::ref_ptr<osg::MatrixTransform> mt1  = new osg::MatrixTransform();

  mt0->setMatrix(osg::Matrix::translate(osg::Vec3(-25.0f, 0.0f, 0.0f)));
  mt1->setMatrix(osg::Matrix::translate(osg::Vec3(25.0f, 0.0f, 0.0f)));

  mt0->addChild(model);
  mt1->addChild(model);

  osg::ref_ptr<osg::Group> root = new osg::Group();
  root->addChild(mt0);
  root->addChild(mt1);

  osg::ref_ptr<osg::Light> light  = new osg::Light();
  light->setDiffuse(osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f));
  mt1->getOrCreateStateSet()->setAttributeAndModes(light);

  osgViewer::Viewer viewer;
  viewer.addEventHandler(new osgViewer::StatsHandler);

  
  viewer.setSceneData(root);

  return viewer.run();
}
