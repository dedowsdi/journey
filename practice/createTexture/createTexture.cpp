#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osg/Geometry>
#include <osgDB/ReadFile>
#include <osg/MatrixTransform>
#include "common.h"
#include "stringutil.h"
#include <osgDB/WriteFile>
#include <osg/LineWidth>
#include <osg/Point>
#include <osg/LineStipple>
#include <osgUtil/Tessellator>
/*
 * used to create texture
 */

osg::ref_ptr<osg::Texture2D> texture;
osg::ref_ptr<osg::Image> image;
osg::ref_ptr<osg::Camera> hudCamera;
std::string fileName;
GLint width = 1024, height = 1024;

class TextureGuiEventHandler : public osgGA::GUIEventHandler {
  virtual bool handle(
    const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) {
    switch (ea.getEventType()) {
      case osgGA::GUIEventAdapter::KEYDOWN:
        switch (ea.getKey()) {
          case osgGA::GUIEventAdapter::KEY_W:
            osgDB::writeImageFile(*image, fileName);
            OSG_NOTICE << "write iamge to " << fileName << std::endl;
            break;
          case osgGA::GUIEventAdapter::KEY_B: {
            osg::Vec4 color = hudCamera->getClearColor();
            color.z() = static_cast<int>(color.z()) ^ 1;
            hudCamera->setClearColor(color);
          }

          default:
            break;
        }
        break;
      default:
        break;
    }
    return false;  // return true will stop event
  }
};

osg::Geode* createCursor() {
  fileName = "cursor.png";
  GLfloat arrowSize = height * 0.5f;
  GLfloat radius = arrowSize * 0.5f;
  GLfloat halfRadius = 0.5 * radius;
  GLuint numSides = 16;
  GLuint numVertices = numSides * 2 + 8;

  osg::ref_ptr<osg::Geometry> cursor = new osg::Geometry;
  osg::ref_ptr<osg::Geode> leaf = new osg::Geode();

  osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array();
  vertices->reserve(numVertices);
  osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array();
  colors->reserve(numVertices);
  colors->setBinding(osg::Array::BIND_PER_VERTEX);

  osg::Vec4 arrowColor = osg::Vec4(0.0f, 0.0f, 0.0f, 1.0f);
  osg::Vec4 circleColor0 = osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f);
  osg::Vec4 circleColor1 = osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f);

  // create arrow
  vertices->push_back(osg::Vec3(halfRadius, 0, 0));
  vertices->push_back(osg::Vec3(arrowSize, 0, 0));
  vertices->push_back(osg::Vec3(0, halfRadius, 0));
  vertices->push_back(osg::Vec3(0, arrowSize, 0));
  vertices->push_back(osg::Vec3(-halfRadius, 0, 0));
  vertices->push_back(osg::Vec3(-arrowSize, 0, 0));
  vertices->push_back(osg::Vec3(0, -halfRadius, 0));
  vertices->push_back(osg::Vec3(0, -arrowSize, 0));

  GLfloat stepYaw = osg::PI * 2 / numSides;
  osg::Vec3 p0(radius, 0, 0);
  vertices->push_back(p0);
  // create circle
  for (GLuint i = 1; i < numSides; ++i) {
    osg::Vec3 p = osg::Quat(stepYaw * i, osg::Z_AXIS) * p0;
    vertices->push_back(p);  // line end of previous line
    vertices->push_back(p);  // line start of next line
  }
  vertices->push_back(p0);  // line end of last line

  for (int i = 0; i < 8; ++i) colors->push_back(arrowColor);
  for (GLuint i = 0; i < numSides; ++i) {
    colors->push_back(i & 1 ? circleColor1 : circleColor0);
    colors->push_back(i & 1 ? circleColor1 : circleColor0);
  }

  cursor->setVertexArray(vertices);
  cursor->setColorArray(colors);
  cursor->addPrimitiveSet(new osg::DrawArrays(GL_LINES, 0, numVertices));

  leaf->addDrawable(cursor);

  osg::StateSet* ss = leaf->getOrCreateStateSet();
  ss->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
  ss->setAttribute(new osg::LineWidth(2.0f));

  return leaf.release();
}

osg::Geode* createLight() {
  fileName = "light.png";
  GLfloat outerRadius = 0.5f * width;
  GLfloat innerRadius = 0.8f * outerRadius;
  GLfloat dotSize = 0.4f * outerRadius;

  // create circles
  osg::ref_ptr<osg::Geode> leaf = new osg::Geode();
  osg::ref_ptr<osg::Geometry> gmCircle = new osg::Geometry();
  osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array();
  osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array();
  gmCircle->setVertexArray(vertices);
  gmCircle->setColorArray(colors);
  colors->setBinding(osg::Array::BIND_OVERALL);

  colors->push_back(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));

  GLuint numSides = 16;
  GLfloat stepYaw = osg::PI * 2 / numSides;

  osg::Vec3 p0(outerRadius, 0, 0);
  osg::Vec3 p1(innerRadius, 0, 0);

  for (unsigned int i = 0; i < numSides; ++i)
    vertices->push_back(osg::Quat(stepYaw * i, osg::Z_AXIS) * p0);

  for (unsigned int i = 0; i < numSides; ++i)
    vertices->push_back(osg::Quat(stepYaw * i, osg::Z_AXIS) * p1);

  gmCircle->addPrimitiveSet(new osg::DrawArrays(GL_LINE_LOOP, 0, numSides));
  gmCircle->addPrimitiveSet(
    new osg::DrawArrays(GL_LINE_LOOP, numSides, numSides));

  {
    osg::StateSet* ss = gmCircle->getOrCreateStateSet();
    ss->setAttributeAndModes(new osg::LineStipple(1, 0x0f0f));
    ss->setAttributeAndModes(new osg::LineWidth(1.5f));
  }

  // create dot
  osg::ref_ptr<osg::Geometry> gmDot = new osg::Geometry();

  osg::ref_ptr<osg::Vec3Array> vertices2 = new osg::Vec3Array();
  gmDot->setVertexArray(vertices2);
  vertices2->push_back(osg::Vec3());
  gmDot->addPrimitiveSet(new osg::DrawArrays(GL_POINTS, 0, 1));

  {
    osg::StateSet* ss = gmDot->getOrCreateStateSet();
    ss->setAttributeAndModes(new osg::Point(dotSize));
  }

  leaf->addDrawable(gmCircle);
  leaf->addDrawable(gmDot);

  return leaf.release();
}

osg::Geode* createArrow() {
  fileName = "arrow.png";
  osg::ref_ptr<osg::Geode> leaf = new osg::Geode();
  osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry();
  osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array();
  osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array();
  colors->setBinding(osg::Array::BIND_OVERALL);

  colors->push_back(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
  geometry->setVertexArray(vertices);

  osg::Vec3 p0(width * 0.1f, -height * 0.4f, 0);
  osg::Vec3 p1(p0.x(), height * 0.2f, 0);
  osg::Vec3 p2(width * 0.38f, height * 0.1f, 0);
  osg::Vec3 p3(width * 0.42f, height * 0.3f, 0);
  osg::Vec3 p4(0, height * 0.5f, 0);
  osg::Vec3 p5(-p3.x(), p3.y(), 0);
  osg::Vec3 p6(-p2.x(), p2.y(), 0);
  osg::Vec3 p7(-p1.x(), p1.y(), 0);
  osg::Vec3 p8(-p0.x(), p0.y(), 0);

  vertices->push_back(p0);
  vertices->push_back(p1);
  vertices->push_back(p2);
  vertices->push_back(p3);
  vertices->push_back(p4);
  vertices->push_back(p5);
  vertices->push_back(p6);
  vertices->push_back(p7);
  vertices->push_back(p8);

  geometry->addPrimitiveSet(new osg::DrawArrays(GL_POLYGON, 0, 9));

  osg::ref_ptr<osgUtil::Tessellator> tl = new osgUtil::Tessellator();
  tl->setTessellationType(osgUtil::Tessellator::TESS_TYPE_POLYGONS);
  tl->setWindingType(osgUtil::Tessellator::TESS_WINDING_ODD);
  tl->retessellatePolygons(*geometry);

  leaf->addDrawable(geometry);
  return leaf.release();
}

osg::GraphicsContext* createGraphicsContext() {
  osg::ref_ptr<osg::GraphicsContext::Traits> traits =
    new osg::GraphicsContext::Traits();

  GLuint cxScreen, cyScreen;
  zxd::getScreenResolution(cxScreen, cyScreen);

  traits->width = width;
  traits->height = height;
  traits->x = (cxScreen - width) * 0.5f;
  traits->y = (cyScreen - height) * 0.5f;
  traits->windowDecoration = false;
  traits->sharedContext = 0;
  traits->doubleBuffer = true;
  traits->alpha = 8;  // enable alpha channel

  osg::ref_ptr<osg::GraphicsContext> gc =
    osg::GraphicsContext::createGraphicsContext(traits);

  return gc.release();
}

int main(int argc, char* argv[]) {
  osg::ArgumentParser ap(&argc, argv);
  osg::ref_ptr<osg::Group> root = new osg::Group();

  if (ap.argc() == 1) {
    OSG_WARN << "it shoud be " << ap[0] << " something " << std::endl;
    return 0;
  }

  osg::Node* geometry = 0;

  if (ap.argc() >= 4) {
    width = zxd::StringUtil::parseInt(ap[2]);
    height = zxd::StringUtil::parseInt(ap[3]);
  }

  osg::GraphicsContext* gc = createGraphicsContext();

  if (strcmp(ap[1], "cursor") == 0) {
    geometry = createCursor();
  } else if (strcmp(ap[1], "light") == 0) {
    geometry = createLight();
  } else if (strcmp(ap[1], "arrow") == 0) {
    geometry = createArrow();
  }

  osg::StateSet* ss = geometry->getOrCreateStateSet();
  ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

  texture = new osg::Texture2D();
  image = new osg::Image;

  // don't need to allocate ?
  // image->allocateImage(width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE);

  texture->setTextureWidth(width);
  texture->setTextureHeight(height);

  hudCamera = zxd::createHUDCamera(
    -width * 0.5f, width * 0.5f, -height * 0.5f, height * 0.5f);
  // hudCamera->attach(osg::Camera::COLOR_BUFFER, texture);
  hudCamera->attach(osg::Camera::COLOR_BUFFER, image);
  hudCamera->addChild(geometry);
  hudCamera->setClearColor(osg::Vec4(0.0f, 0.0f, 0.0f, 0.0f));
  hudCamera->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  hudCamera->setGraphicsContext(gc);

  // hudCamera->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER);
  // hudCamera->setClearColor(osg::Vec4());

  root->addChild(hudCamera);

  osgViewer::Viewer viewer;
  viewer.addEventHandler(new osgViewer::StatsHandler);
  viewer.addEventHandler(new TextureGuiEventHandler);
  viewer.setSceneData(root);
  viewer.getCamera()->setGraphicsContext(gc);
  viewer.getCamera()->setViewport(0, 0, width, height);

  return viewer.run();
}
