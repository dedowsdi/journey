#include <osgParticle/ParticleSystem>
#include <osgParticle/ParticleSystemUpdater>
#include <osgParticle/ModularEmitter>
#include <osg/Point>
#include <osgParticle/RandomRateCounter>
#include <osgParticle/ModularProgram>
#include <osgParticle/AccelOperator>
#include <osgViewer/Viewer>
#include <osg/Texture2D>
#include <osgDB/ReadFile>
#include <osg/PointSprite>
#include <osg/BlendFunc>
#include <osgParticle/range>
#include <osgParticle/LinearInterpolator>
#include "common.h"
#include <osg/io_utils>
#include "zmath.h"

namespace zxd {

class UniverseCullCallback : public osg::Drawable::CullCallback {
  virtual bool cull(osg::NodeVisitor*, osg::Drawable* drawable, osg::State* state) const {
      //adjust particle to look at camera, with y up
      const osgParticle::ParticleSystem* ps = static_cast<const osgParticle::ParticleSystem*>(drawable);
      const osg::Matrix& modelView = state->getModelViewMatrix();

      GLuint count = ps->numParticles();

      for (GLuint i = 0; i < count; ++i) {
        const osgParticle::Particle* particle = ps->getParticle(i);
        if(!particle->isAlive())
          continue;

        osg::Vec3 z = particle->getPosition() * modelView;
        osg::Vec3 up = particle->getVelocity() * modelView;
        osg::Vec3 right = up^z;
        up =  z ^ right; 
        osg::Matrix m( 
            right.x(), right.y(), right.z(), 0, 
            up.x(), up.y(), up.z(), 0,
            z.x(), z.y(), z.z(), 0,
            0, 0, 0, 1
            );
        osg::Vec3 xyz = zxd::Math::getEulerXYZ(m);
        const_cast<osgParticle::Particle*>(particle)->setAngle(osg::Vec3(xyz.z(), xyz.y(), xyz.x()));
      }

    return false;
  }
};


class RandColorParticleSystem : public osgParticle::ParticleSystem {
public:
  RandColorParticleSystem() : osgParticle::ParticleSystem() {}
  RandColorParticleSystem(const ParticleSystem& copy,
    const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY)
      : osgParticle::ParticleSystem(copy, copyop) {}

  META_Object(zxd, RandColorParticleSystem);

  virtual osgParticle::Particle* createParticle(
    const osgParticle::Particle* ptemplate) {
    osgParticle::Particle* particle =
      osgParticle::ParticleSystem::createParticle(ptemplate);
    osg::Vec4 v0 = zxd::Math::randomVector4(0.0f, 1.0f);
    v0.w() = 1.0f;
    osg::Vec4 v1 = zxd::Math::randomVector4(0.0f, 1.0f);
    v1.w() = 1.0f;
    particle->setColorRange(osgParticle::rangev4(v0, v1));
    return particle;
  }
};
}

int main(int argc, char* argv[]) {
  osg::ref_ptr<zxd::RandColorParticleSystem> ps =
    new zxd::RandColorParticleSystem();
  //ps->setDrawCallback(new zxd::UniverseDrawableDrawCallback);
  //ps->setCullCallback(new zxd::UniverseCullCallback);

  osgParticle::Particle& dp = ps->getDefaultParticleTemplate();
  dp.setLifeTime(5.0f);
  dp.setSizeRange(osgParticle::rangef(0.5f, 0.5f));
  dp.setShape(osgParticle::Particle::QUAD);
  // dp.setShape(osgParticle::Particle::LINE);
  dp.setAngle(osg::Vec3(0, 0, osg::PI_2));

  osg::StateSet* ss = ps->getOrCreateStateSet();

  osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D();
  osg::ref_ptr<osg::Image> image =
    osgDB::readImageFile("Images/flaretrail.png");
  texture->setImage(image);
  ss->setTextureAttributeAndModes(0, texture);
  // ss->setAttributeAndModes(new osg::Point(50.0f));
  // ss->setTextureAttributeAndModes(0, new osg::PointSprite );

  osg::ref_ptr<osg::BlendFunc> blendFunc = new osg::BlendFunc();
  blendFunc->setFunction(GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR);
  ss->setAttributeAndModes(blendFunc);
  ss->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

  ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

  // create emitter
  osg::ref_ptr<osgParticle::ModularEmitter> emitter =
    new osgParticle::ModularEmitter();
  emitter->setParticleSystem(ps);

  static_cast<osgParticle::RandomRateCounter*>(emitter->getCounter())
    ->setRateRange(500, 2000);

  osg::ref_ptr<osgParticle::RadialShooter> shooter =
    new osgParticle::RadialShooter();
  emitter->setShooter(shooter);
  shooter->setThetaRange(0, osg::PI_2);
  shooter->setPhiRange(0, osg::PI * 2);
  shooter->setInitialSpeedRange(osgParticle::rangef(1.0f, 2.0f));
  //shooter->setInitialRotationalSpeedRange(
    //osg::Vec3(0.0f, 0.0f, 1.0f), osg::Vec3(0.0f, 0.0f, 3.0f));

  // create program
  osg::ref_ptr<osgParticle::ModularProgram> program =
    new osgParticle::ModularProgram();
  program->setParticleSystem(ps);
  //program->setReferenceFrame(osgParticle::ParticleProcessor::RELATIVE_RF);

  //osg::ref_ptr<osgParticle::AccelOperator> accel =
    //new osgParticle::AccelOperator();
  //accel->setAcceleration(osg::Vec3(0.0f, 0.0f, 2.0f));
  //program->addOperator(accel);


  // create psu
  osg::ref_ptr<osgParticle::ParticleSystemUpdater> psu =
    new osgParticle::ParticleSystemUpdater();
  psu->addParticleSystem(ps);

  osg::ref_ptr<osg::Geode> leaf = new osg::Geode();
  leaf->addDrawable(ps);

  osg::ref_ptr<osg::Group> root = new osg::Group();
  root->addChild(leaf);

  root->addChild(ps);
  root->addChild(emitter);
  root->addChild(program);
  root->addChild(psu);


  osgViewer::Viewer viewer;
  viewer.setSceneData(root);
  return viewer.run();
}
