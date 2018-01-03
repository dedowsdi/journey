#include <osg/Switch>
#include <osgDB/ReadFile>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>

/*
 *switch every 60 frames
 */
class AnimatingSwitch : public osg::Switch {
public:
  AnimatingSwitch() : osg::Switch(), _count(0) {}
  AnimatingSwitch(const AnimatingSwitch& copy,
    const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY)
      : osg::Switch(copy, copyop), _count(copy._count) {}
  META_Node(osg, AnimatingSwitch);

  virtual void traverse(osg::NodeVisitor& nv) {
    if (!((++_count) % 600)) {
      setValue(0, !getValue(0));
      setValue(1, !getValue(1));
    }
    osg::Switch::traverse(nv);
  }

protected:
  unsigned int _count;
};

int main(int argc, char* argv[]) {
  //particle is invisible unless model2 is visible at the beginning, don't know y
  osg::ref_ptr<osg::Node> model1 = osgDB::readNodeFile("cessna.osg");
  osg::ref_ptr<osg::Node> model2 = osgDB::readNodeFile("cessnafire.osg");
  osg::ref_ptr<AnimatingSwitch> root = new AnimatingSwitch;
  root->addChild(model1.get(), false);
  root->addChild(model2.get(), true);

  osgViewer::Viewer viewer;
  viewer.setSceneData(root.get());
  viewer.addEventHandler(new osgViewer::StatsHandler());

  return viewer.run();
}
