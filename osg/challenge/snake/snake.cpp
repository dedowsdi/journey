#include <osgViewer/Viewer>
#include <osgViewer/config/SingleWindow>
#include <osgDB/ReadFile>
#include <osg/MatrixTransform>
#include "zmath.h"

#define WIDTH 800.0f
#define HEIGHT 800.0f
#define CELL_SIZE 80
#define HALF_SIZE CELL_SIZE*0.5
#define CX_CELL WIDTH/CELL_SIZE
#define CY_CELL HEIGHT/CELL_SIZE
#define COOLDOWN 0.15

osg::ref_ptr<osg::Geometry> geometry;

void createGeometry()
{
  geometry = new osg::Geometry();

  osg::ref_ptr<osg::Vec2Array> vertices  = new osg::Vec2Array(osg::Array::BIND_PER_VERTEX);
  GLfloat hz = CELL_SIZE * 0.5 * 0.9;
  vertices->push_back(osg::Vec2(-hz, hz));
  vertices->push_back(osg::Vec2(-hz, -hz));
  vertices->push_back(osg::Vec2(hz, hz));
  vertices->push_back(osg::Vec2(hz, -hz));

  geometry->setVertexArray(vertices);
  geometry->addPrimitiveSet(new osg::DrawArrays(GL_TRIANGLE_STRIP, 0, 4));
}

struct cell : public osg::MatrixTransform
{
public:
  cell(const osg::Vec2i& p)
  {
    updatePos(p);
    osg::ref_ptr<osg::Geode> leaf  = new osg::Geode();
    leaf->addDrawable(geometry);
    this->addChild(leaf);
  }

  void updatePos(const osg::Vec2i& p)
  {
    gpos = p;
    osg::Vec2 translation  = osg::componentMultiply(osg::Vec2(gpos.x(), gpos.y()) + osg::Vec2(0.5, 0.5), osg::Vec2(CELL_SIZE, CELL_SIZE));
    setMatrix(osg::Matrix::translate(translation.x(), translation.y(), 0));
  }
  osg::Vec2i gpos; //grid position
};

typedef std::vector<osg::ref_ptr<cell>> cell_vector;

class snake : public osg::Group
{
protected:
  GLboolean m_eating;
  cell_vector m_body;
  osg::ref_ptr<cell> m_cell;        // newly spawned cell
  osg::ref_ptr<cell> m_eating_cell; // current eating cell
  GLfloat m_cool_down;
  osg::Vec2i m_dir;
  osg::Vec2i m_next_dir;

public:

  snake()
  {
  }

  void next_dir(const osg::Vec2i& v)
  {
    m_next_dir = v;
  }

  void reset()
  {
    std::cout << "reset" << std::endl;
    m_eating = GL_FALSE;
    m_cool_down = COOLDOWN;
    m_next_dir = osg::Vec2i(1, 0);
    
    m_body.clear();
    this->removeChildren(0, this->getNumChildren());

    osg::Vec2i pos(5,1);
    m_body.push_back(new cell(pos));
    this->addChild(m_body.back());
    pos.x() -= 1;
    m_body.push_back(new cell(pos));
    this->addChild(m_body.back());
    pos.x() -= 1;
    m_body.push_back(new cell(pos));
    this->addChild(m_body.back());
    pos.x() -= 1;
    m_body.push_back(new cell(pos));
    this->addChild(m_body.back());

    spawn_cell();
  }

  cell* head()
  {
    return m_body.front();
  }

  void update(GLfloat dt)
  {
    m_cool_down -= dt;
    if(m_cool_down > 0)
      return;

    m_cool_down = COOLDOWN;

    if(m_next_dir + m_dir != osg::Vec2i(0,0))
      m_dir = m_next_dir;

    if(death())
    {
      reset();
      return;
    }

    // digest to tail
    if(m_eating)
    {
      m_eating = GL_FALSE;
      m_body.push_back(m_eating_cell);
    }

    // shift forward
    for(auto iter = m_body.rbegin(); iter != m_body.rend() - 1; ++iter)
    {
      (*iter)->updatePos((*(iter+1))->gpos);
    }

    head()->updatePos(head()->gpos + m_dir);

    eat();
  }

  // dead or not in the forth coming move
  bool death()
  {
    osg::Vec2i next_pos = head()->gpos + m_dir;
    if(next_pos.x() < 0 || next_pos.x() >= CX_CELL || next_pos.y() < 0 || next_pos.y() >= CY_CELL)
    {
      std::cout << "hit border" << std::endl;
      return true;
    }

    if(hit(next_pos, m_body.begin()+1, m_body.end()-1))
    {
      std::cout << "hit self" << std::endl;
      return true;
    }

    return false;
  }

  void spawn_cell()
  {
    if(m_body.size() == CX_CELL * CY_CELL)
      return;

    // gather empty area
    std::list<osg::Vec2i> pool;
    for (size_t i = 0; i < CY_CELL; ++i) {
      for (size_t j = 0; j < CX_CELL; ++j) {
        pool.push_back(osg::Vec2i(j, i)); // store (col, row) as (x, y)
      }
    }
    
    // dig out grids occupied by snake
    std::vector<GLuint> garbage;
    for (size_t i = 0; i < m_body.size(); ++i) 
    {
      GLuint index = m_body[i]->gpos.y() * CY_CELL + m_body[i]->gpos.x(); 
      garbage.push_back(index);
    }
    std::sort(garbage.begin(), garbage.end());

    for(auto iter = garbage.rbegin(); iter != garbage.rend(); ++iter){
      auto i = pool.begin();
      std::advance(i, *iter);
      pool.erase(i);
    }

    auto iter = pool.begin(); std::advance(iter, zxd::Math::randomValue() * pool.size());
    m_cell = new cell(*iter);

    this->addChild(m_cell);
    std::cout << "spawn cell at " << m_cell->gpos.x() << ":" << m_cell->gpos.y()<< std::endl;
  }

  void eat()
  {
    if(m_cell->gpos == head()->gpos)
    {
      m_eating = true;
      m_eating_cell = m_cell;
      spawn_cell();
    }
  }

  bool hit(const osg::Vec2i& pos, cell_vector::iterator beg, cell_vector::iterator end)
  {
    while(beg != end)
    {
      if((*beg)->gpos == pos)
        return true;
      ++beg;
    }

    return false;
  }
};

class SnakeUpdateCallback : public osg::NodeCallback {
public:
  SnakeUpdateCallback(snake* s):
    mSnake(s)
  {
  }

protected:
  osg::ref_ptr<snake> mSnake;

  virtual void operator()(osg::Node* node, osg::NodeVisitor* nv) {
    static GLfloat lastTime = nv->getFrameStamp()->getReferenceTime();
    GLfloat currentTime = nv->getFrameStamp()->getReferenceTime();
    GLfloat deltaTime = currentTime - lastTime;
    mSnake->update(deltaTime);
    traverse(node,nv);
    lastTime = currentTime;
  }
};

class SnakeGuiEventHandler : public osgGA::GUIEventHandler {
public:
  SnakeGuiEventHandler(snake* s):
    mSnake(s)
  {
  }

protected:
  osg::ref_ptr<snake> mSnake;
  virtual bool handle(
    const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) {
    switch (ea.getEventType()) {
      case osgGA::GUIEventAdapter::KEYDOWN:
        switch (ea.getKey()) {
          case osgGA::GUIEventAdapter::KEY_J:
            mSnake->next_dir(osg::Vec2i(-1, 0));
            break;
          case osgGA::GUIEventAdapter::KEY_L:
            mSnake->next_dir(osg::Vec2i(1, 0));
            break;
          case osgGA::GUIEventAdapter::KEY_K:
            mSnake->next_dir(osg::Vec2i(0, -1));
            break;
          case osgGA::GUIEventAdapter::KEY_I:
            mSnake->next_dir(osg::Vec2i(0, 1));
            break;
          default:
            break;
        }
        break;
      default:
        break;
    }
    return false; //return true will stop event
  }
};

int main(int argc, char *argv[])
{
  createGeometry();
  osgViewer::Viewer viewer;
  viewer.apply(new osgViewer::SingleWindow(100, 100, 800, 800));

  osg::Camera* camera = viewer.getCamera();
  camera->setClearColor(osg::Vec4(0.15, 0.15, 0.15, 1));
  camera->setAllowEventFocus(false);
  camera->setViewMatrix(osg::Matrix::identity());
  osg::Matrix m = osg::Matrix::ortho2D(0, WIDTH, 0, HEIGHT);
  camera->setProjectionMatrix(m);

  osg::ref_ptr<osg::Group> root  = new osg::Group();
  osg::ref_ptr<snake> snake0 = new snake();
  root->addChild(snake0);
  snake0->reset();

  root->addUpdateCallback(new SnakeUpdateCallback(snake0));
  viewer.setSceneData(root);
  viewer.addEventHandler(new SnakeGuiEventHandler(snake0));

  return viewer.run();
}
