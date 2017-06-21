#include <osg/PolygonMode>
#include <osgText/Font>
#include <osgViewer/View>

#include "common.h"

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

  wsi->getScreenResolution(
    osg::GraphicsContext::ScreenIdentifier(screenIdentifier), width, height);
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

osg::Camera* createHUDCamera(
  double left, double right, double bottom, double top) {
  osg::ref_ptr<osg::Camera> camera = new osg::Camera();

  camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
  camera->setAllowEventFocus(false);  // no event
  // clear depth only, make sure text render on current graph
  camera->setClearMask(GL_DEPTH_BUFFER_BIT);
  camera->setRenderOrder(osg::Camera::POST_RENDER);
  camera->setProjectionMatrix(osg::Matrix::ortho2D(left, right, bottom, top));

  return camera.release();
}

//------------------------------------------------------------------------------
osg::Camera* createHUDCamera(GLuint screenIdentifier /*= 0*/) {
  GLuint width = 800, height = 600;
  getScreenResolution(width, height);
  return createHUDCamera(0, width, 0, height);
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
  const osg::Vec3& pos, const std::string& content, float size) {
  osg::ref_ptr<osgText::Text> text = new osgText::Text;

  text->setDataVariance(osg::Object::DYNAMIC);
  text->setFont(g_font.get());
  text->setCharacterSize(size);
  text->setAxisAlignment(osgText::TextBase::XY_PLANE);
  text->setPosition(pos);
  text->setText(content);
  return text.release();
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

float randomValue(float min, float max) {
  return (min + (float)rand() / (RAND_MAX + 1.0f) * (max - min));
}

osg::Vec3 randomVector(float min, float max) {
  return osg::Vec3(
    randomValue(min, max), randomValue(min, max), randomValue(min, max));
}

//------------------------------------------------------------------------------
extern osg::Vec4 randomVector4(float min, float max) {
  return osg::Vec4(randomValue(min, max), randomValue(min, max),
    randomValue(min, max), randomValue(min, max));
}

osg::Matrix randomMatrix(float min, float max) {
  osg::Vec3 rot = randomVector(-osg::PI, osg::PI);
  osg::Vec3 pos = randomVector(min, max);
  return osg::Matrix::rotate(
           rot[0], osg::X_AXIS, rot[1], osg::Y_AXIS, rot[2], osg::Z_AXIS) *
         osg::Matrix::translate(pos);
}

//------------------------------------------------------------------------------
osg::Vec3 getEulerXYZ(const osg::Matrixf& mat) {
  // http://www.j3d.org/matrix_faq/
  GLfloat angle_y, D, C, trx, _try, angle_x, angle_z;
  angle_y = D = asin(mat(2, 0)); /* Calculate Y-axis angle */
  C = cos(angle_y);
  // angle_y    *=  RADIANS;
  if (fabs(C) > 0.005) /* Gimball lock? */
  {
    trx = mat(2, 2) / C; /* No, so get X-axis angle */
    _try = -mat(2, 1) / C;
    angle_x = atan2(_try, trx);
    trx = mat(0, 0) / C; /* Get Z-axis angle */
    _try = -mat(1, 0) / C;
    angle_z = atan2(_try, trx);
  } else /* Gimball lock has occurred */
  {
    angle_x = 0;     /* Set X-axis angle to zero */
    trx = mat(1, 1); /* And calculate Z-axis angle */
    _try = mat(0, 1);
    angle_z = atan2(_try, trx);
  }

  /* return only positive angles in [0,360] */
  if (angle_x < 0) angle_x += osg::PI * 2;
  if (angle_y < 0) angle_y += osg::PI * 2;
  if (angle_z < 0) angle_z += osg::PI * 2;

  return osg::Vec3(angle_x, angle_y, angle_z);
}

//------------------------------------------------------------------------------
void removeNodeParents(osg::Node* node, GLuint count) {
  GLuint numParent = node->getNumParents();
  numParent = std::min(numParent, count);

  if (numParent == 0) return;

  while (numParent--) node->getParent(0)->removeChild(node);
}

//------------------------------------------------------------------------------
osg::Matrix arcball(
  const osg::Vec2& np0, const osg::Vec2& np1, GLfloat radius /*= 0.8*/) {
  // get camera point
  osg::Vec3 sp0 = ndcToSphere(np0, radius);
  osg::Vec3 sp1 = ndcToSphere(np1, radius);
  GLfloat rpRadius = 1 / radius;
  // get rotate axis in camera space
  osg::Vec3 axis = sp0 ^ sp1;
  GLfloat theta = acosf(sp0 * sp1 * rpRadius * rpRadius);

  return osg::Matrix::rotate(theta, axis);
}

//------------------------------------------------------------------------------
osg::Vec2 screenToNdc(GLfloat x, GLfloat y, GLfloat cx, GLfloat cy) {
  return osg::Vec2((2 * x - cx) / cx, (cy - 2 * y) / cy);
}

//------------------------------------------------------------------------------
osg::Vec2 screenToNdc(GLfloat nx, GLfloat ny) {
  return osg::Vec2(2 * nx - 1, 2 * ny - 1);
}


//------------------------------------------------------------------------------
osg::Vec3 ndcToSphere(const osg::Vec2& np0, GLfloat radius /*= 0.9f*/) {
  GLfloat len2 = np0.length2();
  GLfloat radius2 = radius * radius;
  if (len2 >= radius2) {
    return osg::Vec3(np0 * (radius / std::sqrt(len2)), 0);
  } else {
    return osg::Vec3(np0, std::sqrt(radius2 - len2));
  }
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
    }
  }
  return false;
}
}
