/*
 *  cancel rotation in cull callback.
 */
#include <osg/MatrixTransform>
#include <osgDB/ReadFile>
#include <osgUtil/CullVisitor>
#include <osgViewer/Viewer>

/*
 * be caerful here. This call back is used to tell billboard node(usually a
 * matrix transform) to cancel parent rotation, it must be added to parent of
 * billboard node.
 */
class BillboardCallback : public osg::NodeCallback {
public:
  BillboardCallback(osg::MatrixTransform* billboard)
      : _billboardNode(billboard) {}

  //update transform of billboardNode during cull visitor in root node
  virtual void operator()(osg::Node* node, osg::NodeVisitor* nv) {
    osgUtil::CullVisitor* cv = dynamic_cast<osgUtil::CullVisitor*>(nv);

    if (_billboardNode.valid() && cv) {
      osg::Vec3d translation, scale;
      osg::Quat rotation, so;
      cv->getModelViewMatrix()->decompose(translation, rotation, scale, so);

      //cancel rotation
      osg::Matrixd matrix(rotation.inverse());
      _billboardNode->setMatrix(matrix);
    }
    traverse(node, nv);
  }

protected:
  osg::observer_ptr<osg::MatrixTransform> _billboardNode;
};

int main(int argc, char** argv) {

  osg::ref_ptr<osg::Group> root = new osg::Group;

  osg::ref_ptr<osg::MatrixTransform> billboardNode = new osg::MatrixTransform;
  root->addChild(osgDB::readNodeFile("lz.osg"));
  billboardNode->addChild(osgDB::readNodeFile("cessna.osg"));
  root->addCullCallback(new BillboardCallback(billboardNode));

  root->addChild(billboardNode.get());

  osgViewer::Viewer viewer;
  viewer.setSceneData(root.get());
  return viewer.run();
}
