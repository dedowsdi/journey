#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgUtil/PrintVisitor>
#include <osgParticle/ParticleSystemUpdater>
#include <osgParticle/ParticleSystem>
#include <osg/BlendFunc>
#include <osg/BlendColor>
#include <osgParticle/ModularEmitter>
#include <osgParticle/ModularProgram>
#include <osgParticle/AccelOperator>
#include <osgParticle/SectorPlacer>
#include <osg/Texture2D>
#include <osg/Point>
#include "glm.h"

#define EXPLOSION_LIFE_TIME 0.8

osg::ref_ptr<osg::Group> root;
osg::ref_ptr<osgParticle::ParticleSystemUpdater> psu;
osg::ref_ptr<osgParticle::ParticleSystem> groundPs;
osg::ref_ptr<osgParticle::ParticleSystem> skyPs;
osg::Camera* filterCamera;

// random particle color
class GroundParticleSystem : public osgParticle::ParticleSystem
{
public:
  osgParticle::Particle* createParticle(const osgParticle::Particle* ptemplate)
  {
    auto p = osgParticle::ParticleSystem::createParticle(ptemplate);
    if(p)
    {
      auto color = glm::linearRand(osg::Vec4(0,0,0,1), osg::Vec4(1,1,1,1));
      p->setColorRange(osgParticle::rangev4(color, color));
    }
    return p;
  }
};

// explode when velocity.z < 0
class GravityOperator : public osgParticle::AccelOperator
{
public:
  void operate(osgParticle::Particle* P, double dt)
  {
    osgParticle::AccelOperator::operate(P, dt);
    if(P->getVelocity().z() < 0)
    {
      P->kill();
      // explode
      GLuint n = glm::linearRand(60, 120);
      while(n--)
      {
        osgParticle::Particle* particle = skyPs->createParticle(0);
        if(particle)
        {
          particle->setPosition(P->getPosition());
          particle->setColorRange(P->getColorRange());
          particle->setVelocity(glm::ballRand(1200));
        }
        else
          break;
      }
    }
  }
};

class FreezeOperator : public osgParticle::Operator
{
public:
  META_Object(::, FreezeOperator);
  FreezeOperator(){}
  FreezeOperator(const FreezeOperator& copy, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY):
    Operator(copy, copyop)
  {
  }

  void operate(osgParticle::Particle* P, double dt)
  {
    P->setVelocity(P->getVelocity() * 0.93);
  }
};


void createGroundParticleSystem()
{
  // create ground particle system
  groundPs = new GroundParticleSystem();
  groundPs->getDefaultParticleTemplate().setShape(osgParticle::Particle::POINT);
  auto color = glm::linearRand(osg::Vec4(0,0,0,1), osg::Vec4(1, 1, 1, 1));
  groundPs->getDefaultParticleTemplate().setColorRange(osgParticle::rangev4(color, color));
  groundPs->getDefaultParticleTemplate().setAlphaRange(osgParticle::rangef(1, 1));
  groundPs->setInitialBound(osg::BoundingBox(-500, -500, 0, 500, 500, 500));

  groundPs->getDefaultParticleTemplate().setLifeTime(0);
  
  osg::ref_ptr<osg::BlendFunc> blendFunc  = new osg::BlendFunc();
  blendFunc->setFunction(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  
  osg::StateSet* ss = groundPs->getOrCreateStateSet();
  ss->setAttributeAndModes(blendFunc);
  ss->setAttribute(new osg::Point(6));
  ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
  ss->setMode(GL_POINT_SMOOTH, osg::StateAttribute::ON);
  ss->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
  
  osg::ref_ptr<osgParticle::ModularEmitter> emitter  = new osgParticle::ModularEmitter();
  emitter->setParticleSystem(groundPs);
  
  osg::ref_ptr<osgParticle::RandomRateCounter> rrc  = new osgParticle::RandomRateCounter();
  rrc->setRateRange(10, 20);
  emitter->setCounter(rrc);
  
  osgParticle::RadialShooter* shooter = static_cast<osgParticle::RadialShooter*>(emitter->getShooter());
  shooter->setThetaRange(-0.2, 0.2);
  shooter->setInitialSpeedRange(800,  1200);

  osg::ref_ptr<osgParticle::SectorPlacer> placer = new osgParticle::SectorPlacer();
  placer->setRadiusRange(0, 500);
  placer->setPhiRange(0, osg::PI * 2);
  emitter->setPlacer(placer);
  
  osg::ref_ptr<osgParticle::ModularProgram> program  = new osgParticle::ModularProgram();
  program->setParticleSystem(groundPs);
  osg::ref_ptr<GravityOperator> accel  = new GravityOperator();
  accel->setToGravity(80);
  program->addOperator(accel);
  
  osg::ref_ptr<osg::Geode> psLeaf  = new osg::Geode();
  psLeaf->addDrawable(groundPs);
  
  root->addChild(psLeaf);
  root->addChild(program);
  root->addChild(emitter);
  psu->addParticleSystem(groundPs);
}

void createSkyParticleSystem()
{
  // create ground particle system
  skyPs = new osgParticle::ParticleSystem();
  skyPs->getDefaultParticleTemplate().setShape(osgParticle::Particle::POINT);
  //skyPs->getDefaultParticleTemplate().setColorRange(osgParticle::rangev4(osg::Vec4(0, 0, 0, 1), osg::Vec4(1, 1, 1, 1)));
  //skyPs->setInitialBound(osg::BoundingBox(-500, -500, 0, 500, 500, 500));

  skyPs->getDefaultParticleTemplate().setLifeTime(EXPLOSION_LIFE_TIME);
  
  osg::ref_ptr<osg::BlendFunc> blendFunc  = new osg::BlendFunc();
  blendFunc->setFunction(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  
  osg::StateSet* ss = skyPs->getOrCreateStateSet();
  ss->setAttributeAndModes(blendFunc);
  ss->setAttribute(new osg::Point(2));
  ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
  ss->setMode(GL_POINT_SMOOTH, osg::StateAttribute::ON);
  ss->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
  
  osg::ref_ptr<osgParticle::ModularProgram> program  = new osgParticle::ModularProgram();
  program->setParticleSystem(skyPs);
  osg::ref_ptr<osgParticle::AccelOperator> accel  = new osgParticle::AccelOperator();
  accel->setToGravity(80);
  osg::ref_ptr<FreezeOperator> freeze  = new FreezeOperator();
  
  program->addOperator(accel);
  program->addOperator(freeze);
  
  osg::ref_ptr<osg::Geode> psLeaf  = new osg::Geode();
  psLeaf->addDrawable(skyPs);
  
  root->addChild(psLeaf);
  root->addChild(program);
  psu->addParticleSystem(skyPs);
}

void createFilter()
{
  filterCamera = new osg::Camera;
  filterCamera->setClearMask(0);
  filterCamera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
  filterCamera->setRenderOrder(osg::Camera::PRE_RENDER);
  filterCamera->setProjectionMatrix(osg::Matrix::identity());
  filterCamera->setViewMatrix(osg::Matrix::identity());

  osg::Geometry* quad = osg::createTexturedQuadGeometry(
      osg::Vec3(-1, -1, 0), osg::Vec3(2, 0, 0), osg::Vec3(0, 2, 0), 0, 0, 1, 1);
  osg::ref_ptr<osg::Geode> leaf  = new osg::Geode();
  leaf->addDrawable(quad);
  filterCamera->addChild(quad);
  root->addChild(filterCamera);

  osg::StateSet* ss = quad->getOrCreateStateSet();
  osg::ref_ptr<osg::BlendFunc> bf = new osg::BlendFunc();
  bf->setFunction(GL_ZERO, GL_CONSTANT_ALPHA);
  ss->setAttributeAndModes(bf);
  ss->setAttributeAndModes(new osg::BlendColor(osg::Vec4(0,0,0,0.65)));

}

int main(int argc, char* argv[])
{
  root = new osg::Group;
  osg::StateSet* ss = root->getOrCreateStateSet();
  ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
  ss->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);

  createFilter();

  psu = new osgParticle::ParticleSystemUpdater;
  root->addChild(psu);
  createGroundParticleSystem();
  createSkyParticleSystem();
  
  osgViewer::Viewer viewer;
  viewer.addEventHandler(new osgViewer::StatsHandler);
  viewer.setSceneData(root);
  osg::Camera* camera = viewer.getCamera();
  camera->setClearMask(0);

  osg::ref_ptr<osgUtil::PrintVisitor> pv = new osgUtil::PrintVisitor(osg::notify(osg::NOTICE));
  root->accept(*pv);
  
  return viewer.run();
}
