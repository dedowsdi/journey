#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgViewer/config/SingleWindow>
#include <osg/MatrixTransform>
#include <osgDB/ReadFile>
#include <osgUtil/PrintVisitor>
#include <osg/Point>
#include <osgParticle/ParticleSystem>
#include <osgParticle/ParticleSystemUpdater>
#include <osgParticle/ModularEmitter>
#include <osgParticle/ModularProgram>
#include <osgParticle/AccelOperator>
#include <osgParticle/range>
#include <osg/BlendFunc>
#include <osg/Point>
#include <osg/PointSprite>
#include <cassert>
#include <iostream>
#include "zmath.h"

#define WIDTH 800.0f
#define HEIGHT 800.0f
#define SIZE 50.0f // invader and ship size
#define SHIP_SPEED 160
#define INVADER_SPEED 50
#define FIRE_COOLDOWN 0.2
#define INVADER_STEP 50
#define BULLET_SPEED 500
#define BULLET_SIZE 15
#define INVADE_DYING_TIME 0.2
#define INVADER_COLOR osg::Vec4(0.5, 0, 1, 1)

class Ship;
class Bullet;
class Invader;

osg::ref_ptr<osg::Geometry> shipGeometry;
osg::ref_ptr<osg::Geometry> bulletGeometry;
osg::ref_ptr<osg::Geometry> invaderGeometry;
osg::ref_ptr<osg::Group> root;
osg::ref_ptr<osg::Group> bulletGroup;
osg::ref_ptr<osg::Group> invaderGroup;
osg::ref_ptr<Ship> ship;
osg::ref_ptr<osgParticle::ParticleSystemUpdater> psu;
osg::ref_ptr<osgText::Text> text;

typedef std::list<osg::ref_ptr<Invader>> InvaderList;
typedef std::list<osg::ref_ptr<Bullet>> BulletList;
InvaderList invaders;
BulletList bullets;


GLint score;

class Base : public osg::MatrixTransform // base for ship, bullet, invader
{
protected:
  bool m_dead;
  osg::Vec2 m_pos;
  osg::Vec3 m_color;
  GLfloat m_size;
  osg::Vec2 m_velocity;
  GLfloat m_speed;
  osg::ref_ptr<osg::Geode> mLeaf;

public:

  virtual ~Base(){}

  Base(GLfloat size):
    m_size(size),
    m_dead(0)
  {
    setMatrix(osg::Matrix::scale(osg::Vec3(m_size, m_size, 1)));
    mLeaf = new osg::Geode;
    addChild(mLeaf);
  }

  bool dead() const { return m_dead; }
  void dead(bool v){ m_dead = v; onDead(); }

  virtual void onDead() {}

  const osg::Vec2& pos() const { return m_pos; }
  void pos(const osg::Vec2& v){ m_pos = v; }
  void pos(GLfloat x, GLfloat y){ m_pos = osg::Vec2(x, y); }

  const osg::Vec3& color() const { return m_color; }
  void color(const osg::Vec3& v){ m_color = v; }

  GLfloat size() const { return m_size; }
  void size(GLfloat v){ m_size = v; }

  osg::Vec2 velocity() const { return m_velocity; }
  void velocity(osg::Vec2 v){ m_velocity = v; }

  bool hit(const Base& i)
  {
    GLfloat r = (m_size + i.size())*0.45;
    return (m_pos-i.pos()).length2() <= r*r;
  }

  virtual void update(GLfloat dt)
  {
    m_pos += m_velocity * dt;
    setMatrix(osg::Matrix::scale(osg::Vec3(m_size, m_size, 1)) *
        osg::Matrix::translate(osg::Vec3(m_pos.x(), m_pos.y(), 0)));
  }

  virtual void reset()
  {
    m_dead = 0;
    m_velocity = osg::Vec2(0, 0);
  }
};

class Invader : public Base
{
protected:
  GLfloat m_angle;
  GLfloat m_dying_timer;
  GLboolean m_dying;
  osg::ref_ptr<osgParticle::ParticleSystem> m_ps;
public :

  Invader():
    Base(SIZE)
  {
    color(osg::Vec3(0.5, 0, 0.5));
    size(SIZE);
    m_angle = zxd::Math::randomValue(0.0f, osg::PI * 2);
    mLeaf->addDrawable(invaderGeometry);
  }

  void update(GLfloat dt)
  {
    if(m_dying)
    {
      m_dying_timer -= dt;
      const osg::BoundingSphere& bs = getBound();
      std::cout << bs.center().x() << " " << bs.center().y() << " " << bs.center().z()
      << "  :  " << bs.radius() << std::endl;
      if(m_dying_timer < 0)
        dead(true);
      return;
    }

    m_angle += dt;
    Base::update(dt);
    _matrix.preMult(osg::Matrix::rotate(m_angle, osg::Vec3(0, 0, 1)));
  }

  void forward()
  {
    m_pos.y() -= INVADER_STEP;
    m_velocity *= -1;
  }

  virtual void reset()
  {
    Base::reset();
    m_speed = INVADER_SPEED;
    m_velocity = osg::Vec2(m_speed, 0);
    m_dying = GL_FALSE;
  }

  GLboolean dying() const { return m_dying; }
  void dying(GLboolean v){ 
    m_dying = v; 
    if(v)
    {
      // hide dying invaders
      mLeaf->setNodeMask(0); 
      m_dying_timer = INVADE_DYING_TIME;

      // creating dying particle
      osg::ref_ptr<osgParticle::ParticleSystem> ps  = new osgParticle::ParticleSystem();
      ps->getDefaultParticleTemplate().setShape(osgParticle::Particle::POINT);
      ps->getDefaultParticleTemplate().setColorRange(osgParticle::rangev4(INVADER_COLOR, INVADER_COLOR));

      osg::ref_ptr<osg::BlendFunc> blendFunc  = new osg::BlendFunc();
      blendFunc->setFunction(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      osg::ref_ptr<osg::Texture2D> texture  = new osg::Texture2D();
      texture->setImage(osgDB::readImageFile("Images/smoke.rgb"));

      osg::StateSet* ss = ps->getOrCreateStateSet();
      ss->setAttributeAndModes(blendFunc);
      ss->setTextureAttributeAndModes(0, texture);
      ss->setAttribute(new osg::Point(6));
      ss->setTextureAttributeAndModes(0, new osg::PointSprite);
      ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
      ss->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

      osg::ref_ptr<osgParticle::ModularEmitter> emitter  = new osgParticle::ModularEmitter();
      emitter->setParticleSystem(ps);

      osg::ref_ptr<osgParticle::RandomRateCounter> rrc  = new osgParticle::RandomRateCounter();
      rrc->setRateRange(60, 120);
      emitter->setCounter(rrc);

      osgParticle::RadialShooter* shooter = static_cast<osgParticle::RadialShooter*>(emitter->getShooter());
      shooter->setThetaRange(osg::PI * 0.5f, osg::PI * 0.5f);
      shooter->setInitialSpeedRange(1, 5);

      osg::ref_ptr<osgParticle::ModularProgram> program  = new osgParticle::ModularProgram();
      program->setParticleSystem(ps);
      osg::ref_ptr<osgParticle::AccelOperator> accel  = new osgParticle::AccelOperator();
      //accel->setAcceleration(osg::Vec3(0, 0, 100));
      //program->addOperator(accel);

      osg::ref_ptr<osg::Geode> psLeaf  = new osg::Geode();
      psLeaf->addDrawable(ps);

      addChild(psLeaf);
      addChild(program);
      addChild(emitter);
      psu->addParticleSystem(ps);
      m_ps = ps;
    }
  }

  void onDead(bool b)
  {
    if(m_ps)
    {
      psu->removeParticleSystem(m_ps);
    }
  }

  //explotion* explode()
  //{
    //return new explotion(m_pos, m_size * 0.25);
  //}
};

class Bullet: public Base
{
public:
  Bullet():
    Base(BULLET_SIZE)
  {
    m_speed = BULLET_SPEED;
    m_velocity = osg::Vec2(0, m_speed);
    mLeaf->addDrawable(bulletGeometry);
  }
};

class Ship : public Base
{
protected:
  GLfloat m_fire_cooldown;
  osg::Vec2i m_dir;

public:

  Ship():
    Base(SIZE),
    m_dir(osg::Vec2i(0,0))
  {
    reset();
    mLeaf->addDrawable(shipGeometry);
  }

  void update(GLfloat dt)
  {
    Base::update(dt);
    m_fire_cooldown -= 0.2;
  }

  Bullet* fire()
  {
    if(m_fire_cooldown > 0)
      return 0;

    m_fire_cooldown = 0.2;
    Bullet* b = new Bullet();
    b->pos(m_pos);
    return b;
  }

  void turn_left(GLboolean b)
  {
    m_dir.x() += b ? -1 : 1;
    update_velocity();
  }

  void turn_right(GLboolean b)
  {
    m_dir.x() += b ? 1 : -1;
    update_velocity();
  }

  void update_velocity()
  {
    m_dir.x() = osg::clampTo(m_dir.x(), -1, 1);
    m_velocity = osg::Vec2(m_dir.x(), m_dir.y()) * m_speed;
  }

  bool hit(const Invader& invader0)
  {
    return false;
  }

  virtual void reset()
  {
    Base::reset();
    color(osg::Vec3(0,0,0));
    m_fire_cooldown = FIRE_COOLDOWN;
    m_speed = SHIP_SPEED;
    pos(WIDTH/2, 50);

    // dont't reset dir here, as user might still hoding keys
    //m_dir = osg::Vec2(0);
    //update_velocity();
  }

};

void resetGame();
void addBullet(Bullet*);
void addInvader(Invader*);
BulletList::iterator removeBullet(BulletList::iterator iter);
InvaderList::iterator removeInvader(InvaderList::iterator iter);

class ShipControl : public osgGA::GUIEventHandler
{
  virtual bool handle(
    const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
    {
    switch (ea.getEventType())
    {
      case osgGA::GUIEventAdapter::KEYDOWN:
        switch (ea.getKey())
        {
          case osgGA::GUIEventAdapter::KEY_A:
            ship->turn_left(true);
            break;

          case osgGA::GUIEventAdapter::KEY_D:
            ship->turn_right(true);
            break;

          case osgGA::GUIEventAdapter::KEY_J:
            {
              addBullet(ship->fire());
            }
            break;

          default:
            break;
        }
        break;
      default:
        break;

      case osgGA::GUIEventAdapter::KEYUP:
        switch(ea.getKey())
        {
          case osgGA::GUIEventAdapter::KEY_A:
            ship->turn_left(false);
            break;

          case osgGA::GUIEventAdapter::KEY_D:
            ship->turn_right(false);
            break;
        }
    }
    return false; //return true will stop event
  }
};


class RootUpate : public osg::NodeCallback
{
  virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
  {
    if(ship->dead() || invaders.empty())
    {
      resetGame();
      return;
    }

    assert(bullets.size() == bulletGroup->getNumChildren());
    assert(invaders.size() == invaderGroup->getNumChildren());

    static GLfloat lastTime = nv->getFrameStamp()->getReferenceTime();
    GLfloat currentTime = nv->getFrameStamp()->getReferenceTime();
    GLfloat deltaTime = currentTime - lastTime;
    lastTime = currentTime;

    for(auto iter = bullets.begin(); iter != bullets.end();){
      if((*iter)->dead())
      {
        score += 100;
        iter = removeBullet(iter);
      }
      else
        ++iter;
    }

    std::stringstream ss;
    ss << score << std::endl;
    text->setText(ss.str());

    for(auto iter = invaders.begin(); iter != invaders.end(); ){
      if((*iter)->dead())
      {
        iter = removeInvader(iter);
      }
      else
        ++iter;
    }

    // animate
    ship->update(deltaTime);
    for(auto iter = bullets.begin(); iter != bullets.end();++iter){
      (*iter)->update(deltaTime);
    }

    // min max invader xy
    osg::Vec2 xy0 = osg::Vec2(WIDTH, HEIGHT);
    osg::Vec2 xy1 = osg::Vec2(0, 0);
    for(auto iter = invaders.begin(); iter != invaders.end(); ++iter ){
      (*iter)->update(deltaTime);
      xy0.x() = std::min(xy0.x(), (*iter)->pos().x());
      xy0.y() = std::min(xy0.y(), (*iter)->pos().y());
      xy1.x() = std::max(xy1.x(), (*iter)->pos().x());
      xy1.y() = std::max(xy1.y(), (*iter)->pos().y());
    }

    // forward if hit border
    if((xy1.x() >= WIDTH && (*invaders.begin())->velocity().x() > 0) ||
       (xy0.x() <= 0 && (*invaders.begin())->velocity().x() < 0))
    {
      for(auto iter = invaders.begin(); iter != invaders.end(); ++iter)
      {
        (*iter)->forward();
      }
    }

    xy0 -= osg::Vec2(SIZE * 0.5, SIZE*0.5);
    xy1 += osg::Vec2(SIZE * 0.5, SIZE*0.5);
    // dead check
    GLfloat hz = BULLET_SIZE * 0.5;
    for(auto iter = bullets.begin(); iter != bullets.end(); ++iter){
      Bullet& b = **iter;
      if(b.pos().x() < 0 - 0.5*hz || b.pos().x() > WIDTH + 0.5 * hz || b.pos().y() > HEIGHT + 0.5 * hz)
      {
        b.dead(true);
        continue;
      }

      // hit test
      if(b.pos().x() < xy0.x() || b.pos().x() > xy1.x() || b.pos().y() < xy0.y() || b.pos().y() > xy1.y())
        continue;

      for(auto iter1 = invaders.begin(); iter1 != invaders.end(); ++iter1){
        if((*iter1)->dying())
          continue;

        if(b.hit(**iter1))
        {
          std::cout << "bullet at " << b.pos().x() << ":" << b.pos().y() << " hit invader at "
          << (*iter1)->pos().x() << ":" << (*iter1)->pos().y() << std::endl;
          b.dead(true);
          (*iter1)->dying(true);
          break;
        }
      }
    }
    
    for(auto iter = invaders.begin(); iter != invaders.end(); ++iter)
    {
      if((*iter)->hit(*ship))
      {
        ship->dead(true);
        break;
      }
    }

    ship->update(deltaTime);

    traverse(node,nv);
  }
};

void createShipGeometry()
{
  shipGeometry = new osg::Geometry;

  osg::ref_ptr<osg::Vec2Array> vertices = new osg::Vec2Array(osg::Array::BIND_PER_VERTEX);
  osg::ref_ptr<osg::Vec3Array> colors = new osg::Vec3Array(osg::Array::BIND_OVERALL);

  vertices->reserve(15);

  // create polygon in griangle fan
  vertices->push_back(osg::Vec2(0, 0));

  vertices->push_back(osg::Vec2(0, 0.5));
  vertices->push_back(osg::Vec2(-0.2, 0.4));
  vertices->push_back(osg::Vec2(-0.2, 0));
  vertices->push_back(osg::Vec2(-0.35, -0.15));
  vertices->push_back(osg::Vec2(-0.2, -0.15));
  vertices->push_back(osg::Vec2(-0.2, -0.3));
  vertices->push_back(osg::Vec2(-0.1, -0.15));
  
  // mirror right part
  for (size_t i = 7; i > 0; --i) {
    vertices->push_back(osg::componentMultiply(vertices->at(i), osg::Vec2(-1, 1)));
  }

  colors->push_back(osg::Vec3(0,0,0));
  
  shipGeometry->setVertexArray(vertices);
  shipGeometry->setColorArray(colors);
  
  shipGeometry->addPrimitiveSet(new osg::DrawArrays(GL_TRIANGLE_FAN, 0, vertices->size()));
}

void createInvaderGeometry()
{
  invaderGeometry = new osg::Geometry;

  osg::ref_ptr<osg::Vec2Array> vertices = new osg::Vec2Array(osg::Array::BIND_PER_VERTEX);
  osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array(osg::Array::BIND_OVERALL);

  vertices->reserve(15);

  GLint num_spides = 10;
  GLfloat step_angle = osg::PI*2 / (num_spides * 2);
  GLfloat innder_radius = 0.15;
  GLfloat outer_radius = 0.45;

  vertices->reserve(num_spides*2 + 1);

  vertices->push_back(osg::Vec2());
  for (int i = 0; i < (num_spides+1)*2; ++i) 
  {
    osg::Matrix m = osg::Matrix::rotate(step_angle * i, osg::Vec3(0, 0, 1));
    if(i & 1)
    {
      osg::Vec4 v = m * osg::Vec4(innder_radius, 0, 0, 1);
      vertices->push_back(osg::Vec2(v.x(), v.y()));
    }
    else if(zxd::Math::randomValue(0.0f, 1.0f) > 0.65f)
    {
      osg::Vec4 v = m * osg::Vec4(outer_radius, 0, 0, 1);
      vertices->push_back(osg::Vec2(v.x(), v.y()));
    }
  }

  colors->push_back(INVADER_COLOR);
  
  invaderGeometry->setVertexArray(vertices);
  invaderGeometry->setColorArray(colors);
  
  invaderGeometry->addPrimitiveSet(new osg::DrawArrays(GL_TRIANGLE_FAN, 0, vertices->size()));
}

void createBulletGeometry()
{
  bulletGeometry = new osg::Geometry;

  osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array();
  osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array(osg::Array::BIND_OVERALL);
  bulletGeometry->setVertexArray(vertices);
  bulletGeometry->setColorArray(colors);

  vertices->push_back(osg::Vec3());
  colors->push_back(osg::Vec4(1, 0, 0, 1));
  
  bulletGeometry->addPrimitiveSet(new osg::DrawArrays(GL_POINTS, 0, 1));
  
  osg::StateSet* ss = bulletGeometry->getOrCreateStateSet();
  ss->setAttributeAndModes(new osg::Point(BULLET_SIZE));
  ss->setMode(GL_POINT_SMOOTH, osg::StateAttribute::ON);
}

void resetGame()
{
  if(!ship)
  {
    ship = new Ship();
    root->addChild(ship);
  }

  score = 0;
  // ship
  ship->reset();
  ship->pos(osg::Vec2(WIDTH/2, 30));

  // place invaders
  GLfloat spacing = 20;
  GLint cx = 10;
  GLint cy = 5;

  GLfloat y0 = HEIGHT - 80;
  GLfloat x0 = (WIDTH - cx * (SIZE) - (cx-1) * spacing)/2 + SIZE/2;

  for(auto iter = invaders.begin(); iter != invaders.end();)
    iter = removeInvader(iter);

  for (int i = 0; i < cx*cy; ++i) {
    Invader* invader = new Invader;
    addInvader(invader);
    GLint row = i/cx;
    GLint col = i%cx;
    GLfloat x= x0 + (SIZE + spacing) * col;
    GLfloat y= y0 - (SIZE + spacing) * row;
    invader->pos(x, y);
    invader->reset();
  }

  for(auto iter = bullets.begin(); iter != bullets.end();)
    iter = removeBullet(iter);

  psu->removeParticleSystem(0, psu->getNumParticleSystems());
}

void addBullet(Bullet* b)
{
  if(!b)
    return;

  if(bulletGroup->getNumChildren() != 0 && bulletGroup->getChildIndex(b) != bulletGroup->getNumChildren())
    std::cout << "error, tring to add the same bullet twice" << std::endl;

  bullets.push_back(b);
  bulletGroup->addChild(b);
}

BulletList::iterator removeBullet(BulletList::iterator iter)
{
  bulletGroup->removeChild(*iter);
  return bullets.erase(iter);
}

void addInvader(Invader* i)
{
  if(!i)
    return;

  if(invaderGroup->getNumChildren() != 0 && invaderGroup->getChildIndex(i) != invaderGroup->getNumChildren())
    std::cout << "error, tring to add the same invader twice" << std::endl;

  invaders.push_back(i);
  invaderGroup->addChild(i);
}

InvaderList::iterator removeInvader(InvaderList::iterator iter)
{
  invaderGroup->removeChild(*iter);
  return invaders.erase(iter);
}

void createText()
{
  text = new osgText::Text;
  text->setFont(osgText::readFontFile("fonts/arial.ttf"));
  text->setCharacterSize(50);
  text->setAxisAlignment(osgText::TextBase::XY_PLANE);
  text->setPosition(osg::Vec3(20, HEIGHT - 50, 0));
  text->setText("");
  text->setDataVariance(osg::Object::DYNAMIC);
}

int main(int argc, char* argv[])
{
  srand(time(0));
  createShipGeometry();
  createInvaderGeometry();
  createBulletGeometry();
  createText();

  root = new osg::Group;
  bulletGroup = new osg::Group;
  invaderGroup = new osg::Group;
  root->addChild(bulletGroup);
  root->addChild(invaderGroup);
  osg::ref_ptr<osg::Geode> textLeaf  = new osg::Geode();
  root->addChild(textLeaf);
  textLeaf->addDrawable(text);
  psu = new osgParticle::ParticleSystemUpdater;
  root->addChild(psu);
  resetGame();

  root->addUpdateCallback(new RootUpate());

  osg::ref_ptr<osgUtil::PrintVisitor> pv = new osgUtil::PrintVisitor(std::cout);
  root->accept(*pv);

  osg::StateSet* ss = root->getOrCreateStateSet();
  ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

  osgViewer::Viewer viewer;
  viewer.apply(new osgViewer::SingleWindow(0, 0, WIDTH, HEIGHT));
  viewer.addEventHandler(new osgViewer::StatsHandler);
  viewer.addEventHandler(new ShipControl());
  viewer.setSceneData(root);

  osg::Camera* camera = viewer.getCamera();
  camera->setCullingMode(camera->getCullingMode() & ~osg::CullSettings::SMALL_FEATURE_CULLING);
  camera->setAllowEventFocus(false);
  camera->setViewMatrix(osg::Matrix::identity());
  camera->setProjectionMatrix(osg::Matrix::ortho2D(0, WIDTH, 0, HEIGHT));

  return viewer.run();
}
