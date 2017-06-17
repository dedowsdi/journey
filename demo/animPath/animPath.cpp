#include <osg/AnimationPath>
#include <osg/MatrixTransform>
#include <osgDB/ReadFile>
#include <osgViewer/Viewer>

//create circle path
osg::AnimationPath* createAnimationPath(float radius, float time) {

  osg::ref_ptr<osg::AnimationPath> path = new osg::AnimationPath;
  path->setLoopMode(osg::AnimationPath::LOOP);

  // number of cuntrol points. The first and last control point should overlap
  unsigned int numSamples = 17;

  float delta_yaw = 2.0f * osg::PI / ((float)numSamples - 1);
  float delta_time = time / (float)(numSamples -1);
  for (unsigned int i = 0; i < numSamples; ++i) {
    float yaw = delta_yaw * (float)i;
    osg::Vec3 pos(sinf(yaw) * radius, cosf(yaw) * radius, 0.0f);
    osg::Quat rot(-yaw, osg::Z_AXIS);
    path->insert(
      delta_time * (float)i, osg::AnimationPath::ControlPoint(pos, rot));
  }
  return path.release();
}

int main(int argc, char* argv[]) {
  // load model, rotate it 90 along z axis
  osg::ref_ptr<osg::Node> model = osgDB::readNodeFile("cessna.osg.0,0,90.rot");
  //must user MatrixTransfrom or it's child class
  osg::ref_ptr<osg::MatrixTransform> root = new osg::MatrixTransform;
  root->addChild(model.get());

  osg::ref_ptr<osg::AnimationPathCallback> apcb =
    new osg::AnimationPathCallback;
  apcb->setAnimationPath(createAnimationPath(50.0f, 6.0f));
  root->setUpdateCallback(apcb.get());

  osgViewer::Viewer viewer;
  viewer.setSceneData(root.get());
  return viewer.run();
}
