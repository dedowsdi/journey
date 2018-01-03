#include <osg/MatrixTransform>
#include <osg/Point>
#include <osg/PointSprite>
#include <osg/Texture2D>
#include <osg/BlendFunc>
#include <osgDB/ReadFile>
#include <osgGA/StateSetManipulator>
#include <osgParticle/ParticleSystem>
#include <osgParticle/ParticleSystemUpdater>
#include <osgParticle/ModularEmitter>
#include <osgParticle/ModularProgram>
#include <osgParticle/AccelOperator>
#include <osgViewer/ViewerEventHandlers>
#include <osgViewer/Viewer>
#include <osgParticle/RadialShooter>

class RotaryNodeCallback : public osg::NodeCallback {
  virtual void operator()(osg::Node* node, osg::NodeVisitor* nv) {
    osgParticle::ModularEmitter* emitter =
      static_cast<osgParticle::ModularEmitter*>(node);
    osgParticle::RadialShooter* shooter =
      static_cast<osgParticle::RadialShooter*>(emitter->getShooter());
    GLfloat t = nv->getFrameStamp()->getReferenceTime();
    shooter->setPhiRange(t, t + 0.01f);
    traverse(node, nv);
  }
};

int main(int argc, char* argv[]) {
  osg::ref_ptr<osgParticle::ParticleSystem> ps =
    new osgParticle::ParticleSystem();
  ps->getDefaultParticleTemplate().setShape(osgParticle::Particle::POINT);

  // setup particle stateset
  osg::StateSet* ss = ps->getOrCreateStateSet();
  ss->setAttribute(new osg::Point(5.0f));
  ss->setTextureAttributeAndModes(0, new osg::PointSprite);

  ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

  osg::ref_ptr<osg::BlendFunc> blendFunc = new osg::BlendFunc();
  blendFunc->setFunction(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  ss->setAttributeAndModes(blendFunc);
  ss->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

  osg::ref_ptr<osg::Texture2D> tex = new osg::Texture2D();
  tex->setImage(osgDB::readImageFile("Images/smoke.rgb"));
  ss->setTextureAttribute(0, tex);

  // create counter
  osg::ref_ptr<osgParticle::RandomRateCounter> rrc =
    new osgParticle::RandomRateCounter();
  rrc->setRateRange(300, 500);

  // create program operator
  osg::ref_ptr<osgParticle::AccelOperator> accel =
    new osgParticle::AccelOperator();
  accel->setToGravity();

  // create program
  osg::ref_ptr<osgParticle::ModularProgram> pp =
    new osgParticle::ModularProgram();
  pp->setParticleSystem(ps);
  pp->addOperator(accel);

  // create emitter
  osg::ref_ptr<osgParticle::ModularEmitter> emitter =
    new osgParticle::ModularEmitter();
  emitter->setCounter(rrc);
  emitter->setParticleSystem(ps);

  osgParticle::RadialShooter* shooter =
    static_cast<osgParticle::RadialShooter*>(emitter->getShooter());
  shooter->setThetaRange(osg::PI_4 + 0.25f, osg::PI_4 + 0.45f);
  shooter->setPhiRange(0.0f, 0.01f);
  shooter->setInitialSpeedRange(15.0f, 20.0f);

  emitter->setUpdateCallback(new RotaryNodeCallback);

  // create updater
  osg::ref_ptr<osgParticle::ParticleSystemUpdater> updater =
    new osgParticle::ParticleSystemUpdater();
  updater->addParticleSystem(ps);

  osg::ref_ptr<osg::Geode> geode = new osg::Geode();
  geode->addDrawable(ps);

  osg::ref_ptr<osg::Group> group = new osg::Group();

  group->addChild(geode);
  group->addChild(emitter);
  group->addChild(pp);
  group->addChild(updater);

  osgViewer::Viewer viewer;
  viewer.setSceneData(group);
  return viewer.run();
}
