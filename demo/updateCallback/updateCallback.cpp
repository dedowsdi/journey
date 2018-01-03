#include <osg/Switch>
#include <osgDB/ReadFile>
#include <osgViewer/Viewer>

//switch every 60 frames
class SwitchingCallback : public osg::NodeCallback {
public:
  SwitchingCallback() : _count(0) {}
  virtual void operator()(osg::Node* node, osg::NodeVisitor* nv) {
    osg::Switch* switchNode = static_cast<osg::Switch*>(node);
    if (!((++_count) % 60) && switchNode) {
      switchNode->setValue(0, !switchNode->getValue(0));
      switchNode->setValue(1, !switchNode->getValue(1));
    }
    //don't forget this
    traverse(node, nv);
  }

protected:
  unsigned int _count;
};

int main(int argc, char* argv[]) {
  osg::ref_ptr<osg::Node> model1 = osgDB::readNodeFile("cessna.osg");
  osg::ref_ptr<osg::Node> model2 = osgDB::readNodeFile("cessnafire.osg");
  osg::ref_ptr<osg::Switch> root = new osg::Switch;
  root->addChild(model1.get(), false);
  root->addChild(model2.get(), true);

  root->setUpdateCallback(new SwitchingCallback);

  osgViewer::Viewer viewer;
  viewer.setSceneData(root.get());
  return viewer.run();
}
