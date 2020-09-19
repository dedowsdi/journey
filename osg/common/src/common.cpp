#include <osg/PolygonMode>
#include <osgText/Font>
#include <osgViewer/View>

#include "common.h"
#include <osg/Point>
#include <stdexcept>

namespace zxd {

//------------------------------------------------------------------------------
void getScreenResolution(
  GLuint& width, GLuint& height, GLuint screenIdentifier /*= 0*/) {
  osg::GraphicsContext::WindowingSystemInterface* wsi =
    osg::GraphicsContext::getWindowingSystemInterface();
  if (!wsi) {
    OSG_WARN << "failed to get window system interface" << std::endl;
    return;
  }

  osg::GraphicsContext::ScreenIdentifier main_screen_id;

  main_screen_id.readDISPLAY();
  main_screen_id.setUndefinedScreenDetailsToDefaultScreen();
  wsi->getScreenResolution(main_screen_id, width, height);

  wsi->getScreenResolution(
    osg::GraphicsContext::ScreenIdentifier(main_screen_id), width, height);
}

//------------------------------------------------------------------------------
osg::Vec2ui getScreenResolution(GLuint screenIdentifier /*= 0*/) {
  osg::Vec2ui v;
  getScreenResolution(v.x(), v.y(), screenIdentifier);
  return v;
}

//--------------------------------------------------------------------
osg::Vec2i getWindowResolution(osgViewer::Viewer& viewer, GLint index)
{
  osgViewer::Viewer::Windows windows;
  viewer.getWindows(windows);
  if (index >= windows.size())
  {
    throw new std::out_of_range("wnd index out of range");
  }
  auto traits = windows[index]->getTraits();
  return osg::Vec2i{traits->width, traits->height};
}

//------------------------------------------------------------------------------
GLdouble getAspectRatio(osg::GraphicsContext* gc) {
  const osg::GraphicsContext::Traits* traits = gc->getTraits();
  return (GLdouble)traits->width / traits->height;
}

//------------------------------------------------------------------------------
GLdouble getScreenAspectRatio() {
  GLuint width, height;
  getScreenResolution(width, height);
  return (double)width / height;
}

osg::ref_ptr<osgText::Font> g_font = osgText::readFontFile("fonts/arial.ttf");

osg::AnimationPathCallback* createAnimationPathCallback(
  float radius, float time) {
  osg::ref_ptr<osg::AnimationPath> path = new osg::AnimationPath;
  path->setLoopMode(osg::AnimationPath::LOOP);

  unsigned int numSamples = 32;
  float delta_yaw = 2.0f * osg::PI / ((float)numSamples - 1.0f);
  float delta_time = time / (float)numSamples;
  for (unsigned int i = 0; i < numSamples; ++i) {
    float yaw = delta_yaw * (float)i;
    osg::Vec3 pos(sinf(yaw) * radius, cosf(yaw) * radius, 0.0f);
    osg::Quat rot(-yaw, osg::Z_AXIS);
    path->insert(
      delta_time * (float)i, osg::AnimationPath::ControlPoint(pos, rot));
  }

  osg::ref_ptr<osg::AnimationPathCallback> apcb =
    new osg::AnimationPathCallback;
  apcb->setAnimationPath(path.get());
  return apcb.release();
}

osg::Camera* createRTTCamera(
  osg::Camera::BufferComponent buffer, osg::Texture* tex, bool isAbsolute) {
  osg::ref_ptr<osg::Camera> camera = new osg::Camera;
  camera->setClearColor(osg::Vec4());
  camera->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  camera->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);
  camera->setRenderOrder(osg::Camera::PRE_RENDER);
  if (tex) {
    tex->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR);
    tex->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR);
    camera->setViewport(0, 0, tex->getTextureWidth(), tex->getTextureHeight());
    camera->attach(buffer, tex);
  }

  if (isAbsolute) {
    camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
    camera->setProjectionMatrix(osg::Matrix::ortho2D(0.0, 1.0, 0.0, 1.0));
    camera->setViewMatrix(osg::Matrix::identity());
    camera->addChild(createScreenQuad(1.0f, 1.0f));
  }
  return camera.release();
}

osg::Camera* createHudCamera(double left, double right, double bottom,
  double top, double near, double far)
{
  auto camera = new osg::Camera();

  camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
  camera->setAllowEventFocus(false);
  camera->setClearMask(GL_DEPTH_BUFFER_BIT);
  camera->setRenderOrder(osg::Camera::POST_RENDER);
  camera->setProjectionMatrix(
    osg::Matrix::ortho(left, right, bottom, top, near, far));
  camera->setViewMatrix(osg::Matrix::identity());

  return camera;
}

//------------------------------------------------------------------------------
osg::Camera* createHudCamera(
  GLuint screenIdentifier /*= 0*/, double near /*= 0*/, double far /*= 100*/) {
  GLuint width = 800, height = 600;
  getScreenResolution(width, height);
  return createHudCamera(0, width, 0, height, near, far);
}

//------------------------------------------------------------------------------
osg::Camera* createFocusHudCamera(double a, double centerX, double centerY,
  double aspectRatio, double near /*= -1*/, double far /*= 1*/) {
  GLuint width = 800, height = 600;
  getScreenResolution(width, height);

  GLdouble halfWidth = 0.5 * a;
  GLdouble halfHeight = 0.5 * a;

  if (aspectRatio >= 1)
    halfWidth *= aspectRatio;
  else
    halfHeight /= aspectRatio;

  return createHudCamera(-halfWidth + centerX, halfWidth + centerX,
    -halfHeight + centerY, halfHeight + centerY, near, far);
}

//------------------------------------------------------------------------------
void setHudCameraFocus(osg::Camera* camera, double a, double centerX,
  double centerY, double aspectRatio, double near /*= -1*/,
  double far /*= 1*/) {
  GLuint width = 800, height = 600;
  getScreenResolution(width, height);

  GLdouble halfWidth = 0.5 * a;
  GLdouble halfHeight = 0.5 * a;

  if (aspectRatio >= 1)
    halfWidth *= aspectRatio;
  else
    halfHeight /= aspectRatio;

  camera->setProjectionMatrix(
    osg::Matrix::ortho(-halfWidth + centerX, halfWidth + centerX,
      -halfHeight + centerY, halfHeight + centerY, near, far));
}

osg::Geode* createScreenQuad(float width, float height, float scale) {
  osg::Geometry* geom =
    osg::createTexturedQuadGeometry(osg::Vec3(), osg::Vec3(width, 0.0f, 0.0f),
      osg::Vec3(0.0f, height, 0.0f), 0.0f, 0.0f, width * scale, height * scale);
  osg::ref_ptr<osg::Geode> quad = new osg::Geode;
  quad->addDrawable(geom);

  int values = osg::StateAttribute::OFF | osg::StateAttribute::PROTECTED;
  quad->getOrCreateStateSet()->setAttribute(
    new osg::PolygonMode(
      osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::FILL),
    values);
  quad->getOrCreateStateSet()->setMode(GL_LIGHTING, values);
  return quad.release();
}

osgText::Text* createText(
  const osg::Vec3& pos, const std::string& content, float size)
{
  auto text = new osgText::Text;

  text->setDataVariance(osg::Object::DYNAMIC);
  text->setFont(g_font.get());
  text->setCharacterSize(size);
  text->setAxisAlignment(osgText::TextBase::XY_PLANE);
  text->setPosition(pos);
  text->setText(content);
  return text;
}

extern osgText::Text* createText(osg::ref_ptr<osgText::Font> font,
  const osg::Vec3& pos, const std::string& content, float size) {
  osg::ref_ptr<osgText::Text> text = new osgText::Text();
  text->setFont(font);
  text->setPosition(pos);
  text->setText(content);
  text->setCharacterSize(size);
  text->setDataVariance(osg::Object::DYNAMIC);
  text->setAxisAlignment(osgText::TextBase::XY_PLANE);
  osg::StateSet* ss = text->getOrCreateStateSet();
  ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
  return text.release();
}

//------------------------------------------------------------------------------
osg::ref_ptr<osg::Geometry> createSingleDot(
  GLfloat pointSize, const osg::Vec4& color) {
  osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry;
  osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array();

  osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array();
  colors->setBinding(osg::Array::BIND_OVERALL);

  geometry->setVertexArray(vertices);
  geometry->setColorArray(colors);

  vertices->push_back(osg::Vec3());
  colors->push_back(color);

  geometry->addPrimitiveSet(new osg::DrawArrays(GL_POINTS, 0, 1));

  osg::StateSet* ss = geometry->getOrCreateStateSet();
  ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

  if (pointSize != 1.0f) {
    osg::ref_ptr<osg::Point> point = new osg::Point();
    point->setSize(pointSize);
    ss->setAttributeAndModes(point);
  }

  return geometry;
}

//------------------------------------------------------------------------------
void removeNodeParents(osg::Node* node, GLuint count) {
  GLuint numParent = node->getNumParents();
  numParent = std::min(numParent, count);

  if (numParent == 0) return;

  while (numParent--) node->getParent(0)->removeChild(node);
}

//------------------------------------------------------------------------------
double getBestFovy() {
  osg::DisplaySettings* ds = osg::DisplaySettings::instance();
  double height = ds->getScreenHeight();
  // double width = ds->getScreenWidth();
  double distance = ds->getScreenDistance();  // what's this
  double vfov = osg::RadiansToDegrees(atan2(height / 2.0f, distance) * 2.0);
  return vfov;
}


//------------------------------------------------------------------------------
osg::Geometry* createPolygonLine(const osg::Vec3& v, GLfloat size) {
  // create polygn line with triangle cut
  osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry;

  return geometry.release();
}

bool PickHandler::handle(
  const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) {
  // only handler ctrl+mlb event
  if (ea.getEventType() != osgGA::GUIEventAdapter::RELEASE ||
      ea.getButton() != osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON ||
      !(ea.getModKeyMask() & osgGA::GUIEventAdapter::MODKEY_CTRL))
    return false;

  osgViewer::View* viewer = dynamic_cast<osgViewer::View*>(&aa);
  if (viewer) {
    osg::ref_ptr<osgUtil::LineSegmentIntersector> intersector =
      new osgUtil::LineSegmentIntersector(
        osgUtil::Intersector::WINDOW, ea.getX(), ea.getY());
    osgUtil::IntersectionVisitor iv(intersector.get());
    viewer->getCamera()->accept(iv);

    if (intersector->containsIntersections()) {
      osgUtil::LineSegmentIntersector::Intersection result =
        *(intersector->getIntersections().begin());
      doUserOperations(result);
    } else {
      doNoHit();
    }
  }
  return false;
}
}
