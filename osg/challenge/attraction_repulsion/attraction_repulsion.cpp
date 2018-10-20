#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgUtil/PrintVisitor>
#include <osg/MatrixTransform>
#include <osg/ShapeDrawable>
#include <osg/LightModel>
#include <osg/Material>

#include "glm.h"

#define G 6.674 * 10000
#define NUM_ATTRACTOR 20
#define SPAWN_PER_FRAME 1

osg::ref_ptr<osg::Group> root;
osg::ref_ptr<osg::ShapeDrawable> sphere;
osg::ref_ptr<osg::Geometry> trail;

std::ostream& operator<< (std::ostream& os, const osg::Vec3& v)
{
  os << v.x()  << " " << v.y() << " " << v.z();
  return os;
}

std::ostream& operator<< (std::ostream& os, const osg::Vec4& v)
{
  os << v.x()  << " " << v.y() << " " << v.z() << " " << v.w();
  return os;
}

class Particle
{
protected:
  GLfloat mMass = 1;
  GLfloat mRadius = 1;
  osg::Vec3 mPos;
  osg::Vec3 mAccel;
  osg::Vec3 mVelocity; 
  osg::Vec3 mLastPos;
  GLint mLife;

public:

  Particle()
  {
    mVelocity = osg::Vec3(0,0,0);
    mAccel = osg::Vec3(0,0,0);
    mLife = 60 * 10;
  }

  // after cross border, lastPos must be reset
  void resetLastPos()
  {
    mLastPos = mPos;
    //std::cout << mLastPos << std::endl;
  }

  void update()
  {
    mLastPos = mPos;
    mPos += mVelocity;
    mVelocity += mAccel;
    mAccel = osg::Vec3(0,0,0);
    --mLife;
  }

  bool dead()
  {
    return mLife <= 0;
  }

  void applyForce(const osg::Vec3& force)
  {
    mAccel += force / mMass;
  }

  const osg::Vec3& pos() const { return mPos; }
  void pos(const osg::Vec3& v){ mPos = v; }

  const osg::Vec3& accel() const { return mAccel; }
  void accel(const osg::Vec3& v){ mAccel = v; }

  const osg::Vec3& velocity() const { return mVelocity; }
  void velocity(const osg::Vec3& v){ mVelocity = v; }

  const osg::Vec3& lastPos() const { return mLastPos; }
  void lastPos(const osg::Vec3& v){ mLastPos = v; }

  GLfloat mass() const { return mMass; }
  void mass(GLfloat v){ mMass = v; }

  GLfloat radius() const { return mRadius; }
  void radius(GLfloat v){ mRadius = v; }
};

class Attractor : public osg::MatrixTransform
{
protected:
  GLfloat mMass = 1;
  GLfloat mRadius = 1;
  osg::Vec3 mPos;

public:

  Attractor()
  {
    osg::ref_ptr<osg::Geode> leaf = new osg::Geode();
    this->addChild(leaf);
    leaf->addDrawable(sphere);
  }

  GLfloat mass() const { return mMass; }
  void mass(GLfloat v){ mMass = v; }

  const osg::Vec3& pos() const { return mPos; }
  void pos(const osg::Vec3& v){ mPos = v;}

  void updateMatrix()
  {
    setMatrix(osg::Matrix::scale(mRadius, mRadius, mRadius) * osg::Matrix::translate(mPos));
  }

  GLfloat radius() const { return mRadius; }
  void radius(GLfloat v){ mRadius = v; }

  void attract(Particle* p)
  {
    osg::Vec3 force = mPos - p->pos();
    GLfloat distance = force.length();
    force /= distance;
    GLfloat r = mRadius + p->radius();
    if(distance <= r)
    {
      force *= -1 * 0.5;
    }else
    {
      force *= G * mMass * p->mass() /(distance * distance);
    }
    p->applyForce(force);
  }
};

typedef std::list<Particle> ParticleList;
ParticleList particles;

typedef std::vector<osg::ref_ptr<Attractor>> AttractorVector;
AttractorVector attractors;

void createGeometry()
{
  sphere = new osg::ShapeDrawable;
  osg::ref_ptr<osg::Sphere> shape = new osg::Sphere();

  osg::ref_ptr<osg::TessellationHints> hint = new osg::TessellationHints();
  hint->setDetailRatio(2);
  sphere->setTessellationHints(hint);
  sphere->setShape(shape);

  trail = new osg::Geometry;
  trail->setDataVariance(osg::Object::DYNAMIC);
  trail->setUseDisplayList(false);
  trail->setInitialBound(osg::BoundingBox(osg::Vec3(-500.0f, -500.0f, -500.0f),
        osg::Vec3(500.0, 500.0, 500.0)));
  osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array();
  osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array(osg::Array::BIND_OVERALL);
  
  trail->setVertexArray(vertices);
  trail->setColorArray(colors);
  colors->push_back(osg::Vec4(0,0,0,0));
  
  osg::StateSet* ss = trail->getOrCreateStateSet();
  ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

  osg::ref_ptr<osg::Geode> leaf = new osg::Geode();
  leaf->addDrawable(trail);
  root->addChild(leaf);
}

void createAttractors()
{
  osg::ref_ptr<osg::Group> group = new osg::Group();
  root->addChild(group);

  osg::StateSet* ss = group->getOrCreateStateSet();

  osg::ref_ptr<osg::Light> light  = new osg::Light();
  light->setPosition(osg::Vec4(1,-1,1,0));
  light->setDiffuse(osg::Vec4(1,1,1,1));
  light->setSpecular(osg::Vec4(0.5,0.5,0.5,0.5));

  osg::ref_ptr<osg::Material> mtl = new osg::Material();
  mtl->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(0.8,0.8,0.8,1));
  mtl->setSpecular(osg::Material::FRONT_AND_BACK, osg::Vec4(0.5,0.5,0.5,1));
  mtl->setShininess(osg::Material::FRONT_AND_BACK, 50);

  osg::ref_ptr<osg::LightModel> lm  = new osg::LightModel();
  lm->setLocalViewer(true);

  ss->setAttributeAndModes(light);
  ss->setAttributeAndModes(mtl);
  ss->setAttributeAndModes(lm);
  ss->setMode(GL_LIGHTING, osg::StateAttribute::ON);
  ss->setMode(GL_RESCALE_NORMAL, osg::StateAttribute::ON);

  for (int i = 0; i < NUM_ATTRACTOR; ++i) 
  {
    Attractor* atr = new Attractor;
    atr->pos(glm::linearRand(osg::Vec3(-200.0f, -200.0f, -200.0f),
          osg::Vec3(200.0f, 200.0f, 200.0f)));
    //std::cout << atr.pos() << std::endl;
    atr->radius(10);
    atr->updateMatrix();
    group->addChild(atr);
    attractors.push_back(atr);
  }
}

class RootUpdate : public osg::NodeCallback
{
  virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
  {
    for (int i = 0; i < SPAWN_PER_FRAME; ++i) {
      Particle p;
      p.pos(glm::linearRand(osg::Vec3(-500.0f, -500.0f, -500.0f),
            osg::Vec3(500.0f,500.0f,500.0f)));
      p.velocity(glm::sphericalRand(2.0f));
      particles.push_back(p);
    }

    osg::Vec3Array& vertices = *static_cast<osg::Vec3Array*>(trail->getVertexArray());
    
    vertices.clear();
    vertices.reserve(particles.size() * 2);
    for(auto iter = particles.begin(); iter != particles.end();)
    {
      if(iter->dead())
      {
        iter = particles.erase(iter);
        continue;
      }

      iter->update();

      for (int i = 0; i < attractors.size(); ++i)
        attractors[i]->attract(&*iter);
      vertices.push_back(iter->lastPos());
      vertices.push_back(iter->pos());
      ++iter;
    }
    trail->removePrimitiveSet(0, trail->getNumPrimitiveSets());
    trail->addPrimitiveSet(new osg::DrawArrays(GL_LINES, 0, vertices.size()));
    vertices.dirty();
    trail->dirtyBound();

    traverse(node,nv);
  }
};

int main(int argc, char* argv[])
{
  root = new osg::Group;
  root->addUpdateCallback(new RootUpdate);
  osg::StateSet* ss = root->getOrCreateStateSet();
  ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
  ss->setMode(GL_CULL_FACE, osg::StateAttribute::ON);
  ss->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);

  createGeometry();
  createAttractors();
  
  osgViewer::Viewer viewer;
  viewer.addEventHandler(new osgViewer::StatsHandler);
  viewer.setSceneData(root);
  viewer.realize();
  //viewer.setLightingMode(osg::View::NO_LIGHT); // no default light

  osg::ref_ptr<osgUtil::PrintVisitor> pv = new osgUtil::PrintVisitor(osg::notify(osg::NOTICE));
  root->accept(*pv);
  
  return viewer.run();
}
