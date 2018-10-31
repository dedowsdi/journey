#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgUtil/PrintVisitor>
#include <osg/MatrixTransform>
#include <osg/Texture2D>
#include <osgText/Text>
#include <sstream>
#include "glm.h"
#include <memory>

class circle_graph;
osg::ref_ptr<osg::Group> root;
osg::ref_ptr<osg::Camera> hudCamera;
osg::ref_ptr<osg::Camera> rttCamera;

osg::ref_ptr<osg::Group> hudRoot;
osg::ref_ptr<osg::Group> textRoot;
osg::ref_ptr<osg::Texture2D> texture;
osg::ref_ptr<osgText::Text> text;

osg::ref_ptr<osg::Geometry> circleGeometry;
osg::ref_ptr<osg::Geometry> trailGeometry;

osg::ref_ptr<circle_graph> rootGraph;

GLint circles = 10;
GLint k = -4;
GLint resolution = 10;
GLint type = 0;
bool clearRtt = false;

class circle_graph : public osg::MatrixTransform
{
protected:
  GLfloat m_angularSpeed = 0;
  GLfloat m_angle = glm::fpi2; // current orbit angle around parent
  GLfloat m_radius;
  osg::Vec2 m_pos = osg::Vec2(0,0); // current position
  osg::Vec2 m_last_pos = osg::Vec2(0,0);

  osg::ref_ptr<circle_graph> m_parent;
  osg::ref_ptr<circle_graph> m_child;

public:

  circle_graph()
  {
    osg::ref_ptr<osg::Geode> leaf = new osg::Geode();
    addChild(leaf);
    leaf->addDrawable(circleGeometry);
  }

  GLfloat angularSpeed() const { return m_angularSpeed; }
  void angularSpeed(GLfloat v){ m_angularSpeed = v; }

  GLfloat radius() const { return m_radius; }
  void radius(GLfloat v){ m_radius = v;}

  const osg::Vec2& pos() const { return m_pos; }
  void pos(const osg::Vec2& v){ m_pos = v; }

  const osg::Vec2& last_pos() const { return m_last_pos; }
  void last_pos(const osg::Vec2& v){ m_last_pos = v; }

  void update()
  {
    if(m_parent)
    {
      m_last_pos = m_pos;
      m_angle += m_angularSpeed;
      GLfloat r = m_radius * (type == 0 ? 1 : -1) + m_parent->radius() ;
      pos(m_parent->pos() +  osg::Vec2(r * cos(m_angle), r * sin(m_angle)));
      updateMatrix();
    }
  
    if(m_child)
      m_child->update();
  }

  void updateMatrix()
  {
    osg::Matrix m = osg::Matrix::scale(osg::Vec3(m_radius, m_radius, 1));
    //OSG_NOTICE << m_pos.x() << ":" << m_pos.y() << std::endl;
    m.postMultTranslate(osg::Vec3(m_pos, 0));
    setMatrix(m);
  }
  
  circle_graph* get_pen()
  {
    if(!m_child)
      return this;

    return m_child->get_pen();
  }

  osg::ref_ptr<circle_graph> parent() const { return m_parent; }
  void parent(osg::ref_ptr<circle_graph> v){ m_parent = v; }

  osg::ref_ptr<circle_graph> child() const { return m_child; }
  void child(osg::ref_ptr<circle_graph> v){ m_child = v; }
};

void updateText()
{
  std::stringstream ss;
  ss << "qQ : circles : " << circles << std::endl;
  ss << "wW : k : " << k << std::endl;
  ss << "eE : resolutions : " << resolution << std::endl;
  ss << "rR : type : " << type << std::endl;
  text->setText(ss.str());
}


void resetGraph()
{
  if(rootGraph)
    hudRoot->removeChildren(0, hudRoot->getNumChildren());

  rootGraph = osg::ref_ptr<circle_graph>(new circle_graph());
  rootGraph->radius(200);
  rootGraph->pos(osg::Vec2(0, 0));
  rootGraph->updateMatrix();
  hudRoot->addChild(rootGraph);

  auto graph = rootGraph;
  for (int i = 1; i < circles; ++i) {
    auto child = osg::ref_ptr<circle_graph>(new circle_graph);
    child->radius(graph->radius() / 3.0);
    GLfloat r = child->radius() * (type == 0 ? 1 : -1) + graph->radius() ;
    child->pos(graph->pos() +  osg::Vec2(0, r));
    child->last_pos(child->pos());
    child->updateMatrix();
    graph->child(child);
    child->parent(graph);
    // be very careful here, child node is direct child of hudcamera, as it
    // didn't inherit scale and rotation from parent
    child->angularSpeed(osg::DegreesToRadians(std::pow(k, i)) / resolution);
    hudRoot->addChild(child);
    graph = child;
  }

  updateText();
  clearRtt = true;
}

class RootUpdate : public osg::NodeCallback
{
  virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
  {
    auto vertices = static_cast<osg::Vec2Array*>(trailGeometry->getVertexArray());
    vertices->clear();
    for (int i = 0; i < resolution; ++i) {
    rootGraph->update();
    auto pen = rootGraph->get_pen();
      vertices->push_back(pen->last_pos());
      vertices->push_back(pen->pos());
    }

    vertices->dirty();
    trailGeometry->dirtyBound();
    osg::DrawArrays* da = static_cast<osg::DrawArrays*>(trailGeometry->getPrimitiveSet(0));
    da->setCount(vertices->size());

    traverse(node,nv);
    if(clearRtt)
    {
      rttCamera->setClearMask(GL_COLOR_BUFFER_BIT);
      clearRtt = false;
    }
    else
    {
      rttCamera->setClearMask(0);
    }
  }
};

void createCircleGeometry()
{

  circleGeometry = new osg::Geometry;
  osg::ref_ptr<osg::Vec2Array> vertices = new osg::Vec2Array();
  osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array(osg::Array::BIND_OVERALL);

  circleGeometry->setVertexArray(vertices);
  circleGeometry->setColorArray(colors);

  GLuint numSlice = 64;
  GLfloat stepAngle = glm::f2pi / numSlice;
  for (int i = 0; i <= numSlice; ++i) {
    GLfloat angle = stepAngle * i;
    vertices->push_back(osg::Vec2(std::cos(angle), std::sin(angle)));
  }
  colors->push_back(osg::Vec4(1, 1, 1, 1));

  circleGeometry->addPrimitiveSet(new osg::DrawArrays(GL_LINE_STRIP, 0, vertices->size()));

  //osg::StateSet* ss = circleGeometry->getOrCreateStateSet();
}

void createTrailGeometry()
{
  trailGeometry = new osg::Geometry;
  trailGeometry->setDataVariance(osg::Object::DYNAMIC);
  osg::ref_ptr<osg::Vec2Array> vertices = new osg::Vec2Array();
  osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array(osg::Array::BIND_OVERALL);
  
  trailGeometry->setVertexArray(vertices);
  trailGeometry->setColorArray(colors);
  colors->push_back(osg::Vec4(1, 0, 1, 1));

  vertices->push_back(osg::Vec2());
  vertices->push_back(osg::Vec2(100, 0));
  
  trailGeometry->addPrimitiveSet(new osg::DrawArrays(GL_LINES, 0, 2));
  trailGeometry->setUseDisplayList(false);
  
  osg::StateSet* ss = trailGeometry->getOrCreateStateSet();
}

class InputHandler : public osgGA::GUIEventHandler
{
  virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
  {
    bool shift = ea.getModKeyMask() & osgGA::GUIEventAdapter::MODKEY_SHIFT;
    bool alt = ea.getModKeyMask() & osgGA::GUIEventAdapter::MODKEY_ALT;
    //bool ctrl = ea.getModKeyMask() & osgGA::GUIEventAdapter::MODKEY_CTRL;
    float factor = 1;
    if(shift)
      factor *= -1;
    if(alt)
      factor *= 10;

    switch (ea.getEventType())
    {
      case osgGA::GUIEventAdapter::KEYDOWN:
        switch (ea.getUnmodifiedKey())
        {
          case osgGA::GUIEventAdapter::KEY_Q:
            circles += 1 * factor;
            resetGraph();
            break;

          case osgGA::GUIEventAdapter::KEY_W:
            k += 1 * factor;
            resetGraph();
            break;
          case osgGA::GUIEventAdapter::KEY_E:
            resolution += 1 * factor;
            resetGraph();
            break;
          case osgGA::GUIEventAdapter::KEY_R:
            type ^= 1;
            resetGraph();
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

int main(int argc, char* argv[])
{
  createCircleGeometry();
  createTrailGeometry();

  root = new osg::Group;
  osg::StateSet* ss = root->getOrCreateStateSet();
  ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
  ss->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
  root->addUpdateCallback(new RootUpdate);

  osgViewer::Viewer viewer;
  viewer.addEventHandler(new osgViewer::StatsHandler);
  viewer.addEventHandler(new InputHandler);
  viewer.setSceneData(root);
  viewer.setLightingMode(osg::View::NO_LIGHT);

  GLuint width, height;
  osg::GraphicsContext::WindowingSystemInterface* wsi = osg::GraphicsContext::getWindowingSystemInterface();
  if(!wsi)
    throw std::runtime_error("0 wsi");
  wsi->getScreenResolution(osg::GraphicsContext::ScreenIdentifier(0), width, height);
  GLfloat ar = static_cast<GLfloat>(width) / height;
  GLfloat w = 500;
  GLfloat h = 500;
  if(ar >= 1)
    w *= ar;
  else
    h /= ar;

  // setup rtt camera to render trail
  rttCamera = new osg::Camera;
  rttCamera->setAllowEventFocus(false);
  rttCamera->setRenderOrder(osg::Camera::PRE_RENDER);
  rttCamera->setClearMask(0);
  rttCamera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
  rttCamera->setProjectionMatrixAsOrtho2D(-w, w, -h, h);
  rttCamera->setViewMatrix(osg::Matrix::identity());
  rttCamera->setViewport(0, 0, width, height);
  rttCamera->setCullingMode(rttCamera->getCullingMode() & ~osg::CullSettings::SMALL_FEATURE_CULLING);
  rttCamera->setClearColor(osg::Vec4(0,0,0,1));
  root->addChild(rttCamera);

  osg::ref_ptr<osg::Geode> trailLeaf = new osg::Geode();
  trailLeaf->addDrawable(trailGeometry);
  rttCamera->addChild(trailLeaf);

  texture = new osg::Texture2D;
  texture->setTextureSize(width, height);
  texture->setInternalFormat(GL_RGBA);
  texture->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
  texture->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
  texture->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR);
  texture->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR);
  rttCamera->attach(osg::Camera::COLOR_BUFFER, texture);
  rttCamera->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);
  rttCamera->setComputeNearFarMode(osgUtil::CullVisitor::DO_NOT_COMPUTE_NEAR_FAR);

  // setup hud camera to render circle and text
  hudCamera = new osg::Camera;
  hudCamera->setAllowEventFocus(false);
  hudCamera->setRenderOrder(osg::Camera::POST_RENDER);
  hudCamera->setClearColor(osg::Vec4(0,0,0,1));
  //hudCamera->setClearMask(0);
  hudCamera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
  hudCamera->setProjectionMatrixAsOrtho2D(-w, w, -h, h);
  hudCamera->setViewMatrix(osg::Matrix::identity());
  hudCamera->setComputeNearFarMode(osgUtil::CullVisitor::DO_NOT_COMPUTE_NEAR_FAR);
  root->addChild(hudCamera);

  osg::ref_ptr<osg::Geode> quadNode = new osg::Geode();
  hudCamera->addChild(quadNode);
  auto quad = osg::createTexturedQuadGeometry(osg::Vec3(-w, -h, 0), osg::Vec3(w*2, 0, 0), osg::Vec3(0, h*2, 0));
  quad->setUseDisplayList(false);
  quadNode->addDrawable(quad);
  {
    osg::StateSet* ss = quadNode->getOrCreateStateSet();
    ss->setTextureAttributeAndModes(0, texture);
  }

  hudRoot = new osg::Group;
  textRoot = new osg::Group;
  hudCamera->addChild(hudRoot);
  hudCamera->addChild(textRoot);

  text = new osgText::Text;
  text->setDataVariance(osg::Object::DYNAMIC);
  text->setFont(osgText::readFontFile("fonts/arial.ttf"));
  text->setCharacterSize(20);
  text->setPosition(osg::Vec3(-w + 20, h - 30, 0));
  osg::ref_ptr<osg::Geode> textLeaf = new osg::Geode();
  textRoot->addChild(textLeaf);
  textLeaf->addDrawable(text);
  //updateText();

  resetGraph();

  osg::ref_ptr<osgUtil::PrintVisitor> pv = new osgUtil::PrintVisitor(osg::notify(osg::NOTICE));
  root->accept(*pv);

  return viewer.run();
}
