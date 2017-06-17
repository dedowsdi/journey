#include <osg/MatrixTransform>
#include <osgAnimation/BasicAnimationManager>
#include <osgAnimation/UpdateMatrixTransform>
#include <osgAnimation/StackedTranslateElement>
#include <osgAnimation/StackedQuaternionElement>
#include <osgDB/ReadFile>
#include <osgViewer/Viewer>

// create path for pos and rot
void createAnimationPath(float radius, float time,
  osgAnimation::Vec3KeyframeContainer* container1,
  osgAnimation::QuatKeyframeContainer* container2) {
  unsigned int numSamples = 5;
  float delta_yaw = 2.0f * osg::PI / ((float)numSamples - 1.0f);
  float delta_time = time / (float)(numSamples - 1);
  for (unsigned int i = 0; i < numSamples; ++i) {
    float yaw = delta_yaw * (float)i;
    osg::Vec3 pos(sinf(yaw) * radius, cosf(yaw) * radius, 0.0f);
    osg::Quat rot(-yaw, osg::Z_AXIS);
    container1->push_back(
      osgAnimation::Vec3Keyframe(delta_time * (float)i, pos));
    container2->push_back(
      osgAnimation::QuatKeyframe(delta_time * (float)i, rot));
  }
}

int main(int argc, char* argv[]) {
  osg::ref_ptr<osgAnimation::Vec3LinearChannel> ch1 =
    new osgAnimation::Vec3LinearChannel;
  ch1->setName("position");            // unique channel name
  ch1->setTargetName("PathCallback");  // name of updater

  osg::ref_ptr<osgAnimation::QuatSphericalLinearChannel> ch2 =
    new osgAnimation::QuatSphericalLinearChannel;
  ch2->setName("quat");
  ch2->setTargetName("PathCallback");  // name of updater

  createAnimationPath(50.0f, 6.0f,
    ch1->getOrCreateSampler()->getOrCreateKeyframeContainer(),
    ch2->getOrCreateSampler()->getOrCreateKeyframeContainer());

  // create animation
  osg::ref_ptr<osgAnimation::Animation> animation = new osgAnimation::Animation;
  animation->setPlayMode(osgAnimation::Animation::LOOP);
  // add channels
  animation->addChannel(ch1.get());
  animation->addChannel(ch2.get());

  osg::ref_ptr<osgAnimation::UpdateMatrixTransform> updater =
    new osgAnimation::UpdateMatrixTransform("PathCallback");  // name of updater
  // the actual action of channel is decided by element in stackedtransform
  updater->getStackedTransforms().push_back(
    new osgAnimation::StackedTranslateElement("position"));  // channel 1 name
  updater->getStackedTransforms().push_back(
    new osgAnimation::StackedQuaternionElement("quat"));  // channel 2 name

  osg::ref_ptr<osg::MatrixTransform> animRoot = new osg::MatrixTransform;
  animRoot->addChild(osgDB::readNodeFile("cessna.osg.0,0,90.rot"));
  animRoot->setDataVariance(osg::Object::DYNAMIC);
  animRoot->setUpdateCallback(updater.get());

  osg::ref_ptr<osgAnimation::BasicAnimationManager> manager =
    new osgAnimation::BasicAnimationManager;
  manager->registerAnimation(animation.get());

  osg::ref_ptr<osg::Group> root = new osg::Group;
  root->addChild(animRoot.get());
  // attach manager to root node
  root->setUpdateCallback(manager.get());

  manager->playAnimation(animation.get());

  osgViewer::Viewer viewer;
  viewer.setSceneData(root.get());
  return viewer.run();
}
