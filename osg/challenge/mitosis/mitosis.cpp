#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osg/ShapeDrawable>
#include <osg/MatrixTransform>
#include <osg/Material>
#include <osg/LightModel>
#include "zmath.h"

// constrain cells in a sphere
#define RADIUS 25.0f
#define MIN_SPEED 0.1f
#define MAX_SPEED 3.0f
#define NUM_CELLS 30
#define MITOSIS_TIME 1.5f
#define MITOSIS_SHRINK 0.8f

osg::ref_ptr<osg::Group> root;
osg::ref_ptr<osg::ShapeDrawable> sphere;
osg::ref_ptr<osg::Shape> shape;

class Cell : public osg::MatrixTransform
{
protected:
  osg::Vec3 m_pos;
  osg::Vec3 m_velocity;
  GLfloat m_radius;
  GLfloat m_split_time;
  GLboolean m_mitosis = GL_FALSE;
  osg::Vec3 m_mitosis_start_pos;
  osg::Vec3 m_mitosis_end_pos;
  GLfloat m_mitosis_start_radius;
  GLfloat m_mitosis_end_radius;

public:

  Cell(GLfloat radius = 3):
    m_radius(radius)
  {
    m_pos.x() = zxd::Math::randomValue(-1, 1);
    m_pos.y() = zxd::Math::randomValue(-1, 1);
    m_pos.z() = zxd::Math::randomValue(-1, 1);
    m_pos.normalize();
    m_pos *= zxd::Math::randomValue(0, RADIUS);

    reset_velocity();

    osg::ref_ptr<osg::Geode> leaf  = new osg::Geode();
    leaf->addDrawable(sphere);
    addChild(leaf);
    updateMatrix();
  }

  void update(GLfloat dt)
  {
    if(m_mitosis)
    {
      m_split_time += dt;
      GLfloat p = m_split_time/MITOSIS_TIME;
      if(p > 1)
      {
        m_mitosis = GL_FALSE;
        m_velocity = m_mitosis_end_pos - m_mitosis_start_pos;
        m_velocity.normalize();
        m_velocity *= zxd::Math::randomValue(MIN_SPEED, MAX_SPEED);
        return;
      }

      m_radius = zxd::Math::mix(m_mitosis_start_radius, m_mitosis_end_radius, zxd::Math::smoothstep(0.0f, 1.0f, p));
      m_pos = zxd::Math::mix(m_mitosis_start_pos, m_mitosis_end_pos, zxd::Math::smoothstep(0.0f, 1.0f, p));
      updateMatrix();
      return;
    }

    m_pos += m_velocity * dt;

    if(m_pos.length2() >= RADIUS * RADIUS && m_pos * m_velocity > 0)
    {
      // reflect
      osg::Vec3 n = -m_pos;
      n.normalize();
      m_velocity.normalize();
      m_velocity *= -zxd::Math::randomValue(MIN_SPEED, MAX_SPEED) ;
    }
    updateMatrix();
  }

  void reset_velocity()
  {
    m_velocity.x() = zxd::Math::randomValue(-1, 1);
    m_velocity.y() = zxd::Math::randomValue(-1, 1);
    m_velocity.z() = zxd::Math::randomValue(-1, 1);
    m_velocity.normalize();
    m_velocity *= zxd::Math::randomValue(MIN_SPEED, MAX_SPEED);
  }

  void updateMatrix()
  {
    setMatrix(osg::Matrix::scale(osg::Vec3(m_radius, m_radius, m_radius)) * osg::Matrix::translate(m_pos));
  }

  osg::Vec3 pos() const { return m_pos; }
  void pos(osg::Vec3 v){ m_pos = v; }

  GLfloat radius() const { return m_radius; }
  void radius(GLfloat v){ m_radius = v; }

  void mitosis(const osg::Vec3& mitosis_dir)
  {
    m_mitosis = GL_TRUE;
    m_mitosis_start_radius = m_radius;
    m_mitosis_end_radius = m_radius * MITOSIS_SHRINK;
    m_mitosis_start_pos = m_pos;
    m_mitosis_end_pos = m_pos + mitosis_dir * m_mitosis_end_radius;
    m_split_time = 0;
  }
};

typedef std::vector<osg::ref_ptr<Cell>> cell_vector;
cell_vector cells;

class RootUpdate : public osg::NodeCallback
{
  virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
  {
    static GLfloat lastTime = nv->getFrameStamp()->getReferenceTime();
    GLfloat currentTime = nv->getFrameStamp()->getReferenceTime();
    GLfloat deltaTime = currentTime - lastTime;
    lastTime = currentTime;

    for(auto iter = cells.begin(); iter != cells.end(); ++iter){
      (*iter)->update(deltaTime);
    }

    traverse(node,nv);
  }
};

class ClickHandler : public osgGA::GUIEventHandler
{
  virtual bool handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
  {
    if(ea.getEventType() != osgGA::GUIEventAdapter::PUSH ||
        ea.getButton() != osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
      return false;

    osgViewer::View* view = dynamic_cast<osgViewer::View*>(&aa);
    if(!view)
      return false;

    osgUtil::IntersectionVisitor iv;
    osg::ref_ptr<osgUtil::LineSegmentIntersector> lsi =
      new osgUtil::LineSegmentIntersector(osgUtil::Intersector::WINDOW, ea.getX(), ea.getY());
    iv.setIntersector(lsi);
    //iv.setTraversalMask(selectMask);
    
    view->getCamera()->accept(iv);
    
    if (lsi->containsIntersections()) 
    {
      osgUtil::LineSegmentIntersector::Intersection result = *lsi->getIntersections().begin();
      Cell* cell = dynamic_cast<Cell*>(*(result.nodePath.begin() + 2));
      if(cell)
      {
        osg::Vec3 mitosis_dir;
        mitosis_dir.x() = zxd::Math::randomValue(-1, 1);
        mitosis_dir.y() = zxd::Math::randomValue(-1, 1);
        mitosis_dir.z() = zxd::Math::randomValue(-1, 1);
        mitosis_dir.normalize();

        cell->mitosis(mitosis_dir);

        osg::ref_ptr<Cell> cell1 = new Cell(cell->radius());
        cell1->pos(cell->pos());
        cell1->mitosis(-mitosis_dir);

        cells.push_back(cell1);
        root->addChild(cell1);
      }
    
    }
    
    return false; //return true will stop event
  }
};
int main(int argc, char* argv[])
{
  shape = new osg::Sphere();
  osg::ref_ptr<osg::TessellationHints> th  = new osg::TessellationHints();
  th->setDetailRatio(2);
  sphere = new osg::ShapeDrawable(shape, th);
  root = new osg::Group;

  osg::StateSet* ss = root->getOrCreateStateSet();
  osg::ref_ptr<osg::Material> mtl  = new osg::Material();
  mtl->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(0, 0.6, 0, 1));
  mtl->setSpecular(osg::Material::FRONT_AND_BACK, osg::Vec4(1, 1, 1, 1));
  mtl->setShininess(osg::Material::FRONT_AND_BACK, 50);
  ss->setAttributeAndModes(mtl);
  ss->setMode(GL_RESCALE_NORMAL, osg::StateAttribute::ON);
  
  osg::ref_ptr<osg::LightModel> lm  = new osg::LightModel();
  lm->setLocalViewer(true);
  ss->setAttributeAndModes(lm);

  root->addUpdateCallback(new RootUpdate);
  for (size_t i = 0; i < NUM_CELLS; ++i) {
    cells.push_back(new Cell);
    root->addChild(cells.back());
  }

  osgViewer::Viewer viewer;
  viewer.addEventHandler(new osgViewer::StatsHandler);
  viewer.setSceneData(root);
  viewer.addEventHandler(new ClickHandler);


  return viewer.run();
}
