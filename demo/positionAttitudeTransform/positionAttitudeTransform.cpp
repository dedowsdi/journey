#include <osgViewer/Viewer>
#include <osg/Geometry>
#include <osgDB/ReadFile>
#include <osg/PositionAttitudeTransform>

class PatNodeCallback : public osg::NodeCallback {
public:
  PatNodeCallback() {}
  virtual void operator()(osg::Node* node, osg::NodeVisitor* nv) {

    osg::PositionAttitudeTransform* pat = node->asTransform()->asPositionAttitudeTransform();
    pat->setPosition(pat->getPosition() + osg::Vec3(0.002f, 0.0f, 0.0f));
    pat->setAttitude(pat->getAttitude() * osg::Quat(osg::PI/1800, osg::Z_AXIS));
    // don't forget this
    traverse(node, nv);
  }
protected:
};

int main(int argc, char* argv[]) {
  osg::ref_ptr<osg::Group> root = new osg::Group();
  osg::ref_ptr<osg::Node> modelNode = osgDB::readNodeFile("cessna.osg");

  osg::ref_ptr<osg::PositionAttitudeTransform> pat =
    new osg::PositionAttitudeTransform();
  pat->addChild(modelNode);

  osg::ref_ptr<PatNodeCallback> callback = new PatNodeCallback();
  pat->setUpdateCallback(callback);

  root->addChild(pat);
  osgViewer::Viewer viewer;
  viewer.setSceneData(root);

  return viewer.run();
}
