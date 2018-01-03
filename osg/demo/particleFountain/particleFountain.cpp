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

osgParticle::ParticleSystem* createParticleSystem(osg::Group* parent) {

  osg::ref_ptr<osgParticle::ParticleSystem> ps =
    new osgParticle::ParticleSystem;
  ps->getDefaultParticleTemplate().setShape(osgParticle::Particle::POINT);

  osg::ref_ptr<osg::BlendFunc> blendFunc = new osg::BlendFunc;
  blendFunc->setFunction(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
  texture->setImage(osgDB::readImageFile("Images/smoke.rgb"));

  osg::StateSet* ss = ps->getOrCreateStateSet();
  ss->setAttributeAndModes(blendFunc.get());
  ss->setTextureAttributeAndModes(0, texture.get());

  //use point sprite to render particle
  ss->setAttribute(new osg::Point(5.0f));
  ss->setTextureAttributeAndModes(0, new osg::PointSprite);

  ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
  ss->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

  //create counter
  osg::ref_ptr<osgParticle::RandomRateCounter> rrc =
    new osgParticle::RandomRateCounter;
  rrc->setRateRange(500, 800);

  //create emitter
  osg::ref_ptr<osgParticle::ModularEmitter> emitter =
    new osgParticle::ModularEmitter;
  emitter->setParticleSystem(ps.get());
  emitter->setCounter(rrc.get());

  //program operator
  osg::ref_ptr<osgParticle::AccelOperator> accel =
    new osgParticle::AccelOperator;
  accel->setToGravity();

  //create program
  osg::ref_ptr<osgParticle::ModularProgram> program =
    new osgParticle::ModularProgram;
  program->setParticleSystem(ps.get());
  program->addOperator(accel.get());

  
  osg::ref_ptr<osg::Geode> geode = new osg::Geode;
  geode->addDrawable(ps.get());
  //Adding only the osg::Geode node to an osg::Transform will make the entire
  //particle system move with it; but adding only the emitter will change the
  //transform behavior of new-born particles but will leave any existing ones in
  //the world coordinate. Similarly, only adding the program node will make the
  //parent transformation node only affect the operators
  parent->addChild(emitter.get());
  parent->addChild(program.get());
  parent->addChild(geode.get());
  return ps.get();
}

int main(int argc, char* argv[]) {

  osg::ref_ptr<osg::MatrixTransform> mt = new osg::MatrixTransform;
  mt->setMatrix(osg::Matrix::translate(1.0f, 0.0f, 0.0f));

  osgParticle::ParticleSystem* ps = createParticleSystem(mt.get());

  //create updater
  osg::ref_ptr<osgParticle::ParticleSystemUpdater> updater =
    new osgParticle::ParticleSystemUpdater;
  updater->addParticleSystem(ps);

  osg::ref_ptr<osg::Group> root = new osg::Group;
  root->addChild(updater.get());
  root->addChild(mt.get());
  root->addChild(osgDB::readNodeFile("axes.osg")); //not found
  osgViewer::Viewer viewer;
  viewer.setSceneData(root.get());
  return viewer.run();
}
