// steer direction = desire - current
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgUtil/PrintVisitor>
#include <osgText/Text3D>
#include <osg/TriangleFunctor>
#include <osg/MatrixTransform>
#include <osg/ShapeDrawable>
#include <osg/Point>
#include <set>
#include "glm.h"

osg::ref_ptr<osg::Group> root;
osg::ref_ptr<osg::Geometry> dot;

struct PointFunctor
{
  std::set<osg::Vec2> vertices;
  void operator()(const osg::Vec3& p0, const osg::Vec3& p1, const osg::Vec3& p2,
      bool treatVertexDataAsTemporary)
  {
    vertices.insert(osg::Vec2(p0.x(), p0.y()));
    vertices.insert(osg::Vec2(p1.x(), p1.y()));
    vertices.insert(osg::Vec2(p2.x(), p2.y()));
  }
};

/*
 * a very inefficient way to get 2d glyph points.
 *
 * see 
 *  FreeTypeFront::getGlyph3D
 *  GlyphGeometry::setup
 *  osgText::computeGlyphGeometry
 * if you want to improve this
 */
std::vector<osg::Vec2> textToPoints(const std::string& content, GLfloat size)
{
  osg::ref_ptr<osgText::Text3D> text = new osgText::Text3D();
  text->setText(content);
  text->setCharacterSize(size);
  text->setFont(osgText::readFontFile("fonts/arial.ttf"));
  text->setCharacterDepth(1); // this will trigger geometry build

  osg::TriangleFunctor<PointFunctor> functor;
  text->accept(functor);

  std::vector<osg::Vec2> vertices;
  vertices.assign(functor.vertices.begin(), functor.vertices.end());
  return vertices;
}

class Vehicle : public osg::MatrixTransform
{
protected:
  GLfloat mSteerSpeed = 15;
  GLfloat mFleeSpeed = 100;
  osg::Vec2 mPos;
  osg::Vec2 mVel;
  osg::Vec2 mAcc;
  osg::Vec2 mTarget;

public:

  Vehicle()
  {
    osg::ref_ptr<osg::Geode> leaf = new osg::Geode();
    leaf->addDrawable(dot);
    addChild(leaf);
  }

  void update()
  {
    mPos += mVel;
    mVel += mAcc;
    mAcc = osg::Vec2();
    updateMatrix();
  }

  void updateMatrix()
  {
    setMatrix(osg::Matrix::translate(osg::Vec3(mPos, 0)));
  }

  // steer to current target
  void steer()
  {
    // get desire speed
    osg::Vec2 desire = mTarget - mPos;
    if(desire == osg::Vec2())
      return;
    GLfloat l = desire.length();
    desire /= l;
    desire *= glm::linearstep(0, 200, l) * mSteerSpeed;

    osg::Vec2 force = desire - mVel;
    applyForce(force);
  }

  void flee(const osg::Vec2& p, GLfloat radius)
  {
    osg::Vec2 desire = mPos - p;
    GLfloat l = desire.length();
    if (l >= radius)
      return;
    desire *= (1 - glm::linearstep(0, radius, l)) * mFleeSpeed / l;

    osg::Vec2 force = desire - mVel;
    applyForce(force);
  }

  void applyForce(const osg::Vec2& force)
  {
    mAcc += force;
  }

  osg::Vec2 pos() const { return mPos; }
  void pos(osg::Vec2 v){ mPos = v; }

  osg::Vec2 vel() const { return mVel; }
  void vel(osg::Vec2 v){ mVel = v; }

  osg::Vec2 acc() const { return mAcc; }
  void acc(osg::Vec2 v){ mAcc = v; }

  osg::Vec2 target() const { return mTarget; }
  void target(osg::Vec2 v){ mTarget = v; }
};

std::vector<osg::ref_ptr<Vehicle>> vehicles;

void createVehicles()
{
  auto points = textToPoints("Winter", 300);
  osg::Vec2 start = osg::Vec2(300, 300);
  for(const auto& item : points)
  {
    osg::ref_ptr<Vehicle> vehicle = new Vehicle();
    vehicle->pos(item + start);
    vehicle->target(vehicle->pos());
    vehicle->updateMatrix();
    vehicles.push_back(vehicle);
    root->addChild(vehicle);
  }
}

void createDot()
{
  dot = new osg::Geometry;
  osg::ref_ptr<osg::Vec2Array> vertices = new osg::Vec2Array();
  osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array(osg::Array::BIND_OVERALL);
  
  dot->setVertexArray(vertices);
  dot->setColorArray(colors);

  colors->push_back(osg::Vec4(1, 1, 1, 1));
  vertices->push_back(osg::Vec2());
  
  dot->addPrimitiveSet(new osg::DrawArrays(GL_POINTS, 0, 1));
  
  osg::StateSet* ss = dot->getOrCreateStateSet();
  ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
  ss->setAttributeAndModes(new osg::Point(2));
  ss->setMode(GL_POINT_SMOOTH, osg::StateAttribute::ON);
}

osg::Node* createPoints(const std::vector<osg::Vec2>& points, const osg::Vec2& pos)
{
  osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry;
  osg::ref_ptr<osg::Vec2Array> vertices = new osg::Vec2Array();
  osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array(osg::Array::BIND_OVERALL);

  geometry->setVertexArray(vertices);
  geometry->setColorArray(colors);

  colors->push_back(osg::Vec4(1,1,1,1));
  vertices->reserve(points.size());
  for (int i = 0; i < points.size(); ++i) {
    vertices->push_back(points[i] + pos);
  }
  
  geometry->addPrimitiveSet(new osg::DrawArrays(GL_POINTS, 0, points.size()));
  
  osg::StateSet* ss = geometry->getOrCreateStateSet();
  ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
  
  osg::Geode* leaf = new osg::Geode();
  leaf->addDrawable(geometry);
  return leaf;
}


class SteerGuiEventHandler : public osgGA::GUIEventHandler
{
protected:
  osg::Vec2 mMouse;
public:

  virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
  {
    switch (ea.getEventType())
    {
      case osgGA::GUIEventAdapter::KEYDOWN:
        switch (ea.getKey())
        {
          case osgGA::GUIEventAdapter::KEY_F1:
            break;
          default:
            break;
        }
        break;

      case osgGA::GUIEventAdapter::MOVE:
        mMouse = osg::Vec2(ea.getX(), ea.getY());
        break;

      default:
        break;
    }
    return false; //return true will stop event
  }

  osg::Vec2 getMouse() const { return mMouse; }
  void setMouse(osg::Vec2 v){ mMouse = v; }
};

osg::ref_ptr<SteerGuiEventHandler> guiHandler;

class RootUpdate : public osg::NodeCallback
{
  virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
  {
    const osg::Vec2& mouse = guiHandler->getMouse();
    for( auto& item : vehicles)
    {
      item->flee(mouse, 100);
      item->steer();
      item->update();
    }
    
    traverse(node,nv);
  }
};


int main(int argc, char* argv[])
{
  root = new osg::Group;
  osg::StateSet* ss = root->getOrCreateStateSet();
  ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
  ss->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
  root->addUpdateCallback(new RootUpdate);

  createDot();
  createVehicles();
  
  osgViewer::Viewer viewer;
  viewer.addEventHandler(new osgViewer::StatsHandler);
  viewer.setSceneData(root);

  guiHandler = new SteerGuiEventHandler;
  viewer.addEventHandler(guiHandler);

  osg::ref_ptr<osgUtil::PrintVisitor> pv = new osgUtil::PrintVisitor(osg::notify(osg::NOTICE));
  root->accept(*pv);

  viewer.realize();

  osg::GraphicsContext::WindowingSystemInterface* wsi = osg::GraphicsContext::getWindowingSystemInterface();
  GLuint width;
  GLuint height;
  wsi->getScreenResolution(osg::GraphicsContext::ScreenIdentifier(0), width, height);

  osg::Camera* camera = viewer.getCamera();
  camera->setAllowEventFocus(false);
  camera->setCullingMode(camera->getCullingMode() & ~osg::CullSettings::SMALL_FEATURE_CULLING);
  camera->setComputeNearFarMode(osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);
  camera->setProjectionMatrixAsOrtho2D(0, width, 0, height);
  camera->setViewMatrix(osg::Matrix::identity());

  //auto points = textToPoints("Winter Is Coming", 100);
  //for(const auto& item : points)
    //OSG_NOTICE << item.x() << " " << item.y() << std::endl;
  //root->addChild(createPoints(points, osg::Vec2(500, 500)));

  return viewer.run();
}
