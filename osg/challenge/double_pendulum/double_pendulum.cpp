#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgUtil/PrintVisitor>
#include <osg/MatrixTransform>
#include "glm.h"

GLuint width, height;
bool clearRtt = true;
GLfloat m1 = 1;
GLfloat m2 = 1;
GLfloat l1 = 30;
GLfloat l2 = 30;
GLfloat theta1 = glm::fpi2;
GLfloat theta2 = glm::fpi4;
GLfloat radius = 2;
GLfloat g = 0.2;

GLfloat av1 = 0;
GLfloat av2 = 0;
GLfloat aa1 = 0;
GLfloat aa2 = 0;

osg::Vec2 origin = osg::Vec2(0, 50);
osg::Vec2 pos1;
osg::Vec2 pos2;
osg::Vec2 last_pos;
osg::Vec2 hext = osg::Vec2(100, 100);

osg::ref_ptr<osg::Group> orthoRoot;
osg::ref_ptr<osg::Group> rttRoot;
osg::ref_ptr<osg::Geometry> rope;
osg::ref_ptr<osg::Geometry> trace;
osg::ref_ptr<osg::Geometry> circle;
osg::ref_ptr<osg::Group> root;
osg::ref_ptr<osg::MatrixTransform> line;
osg::ref_ptr<osg::MatrixTransform> ball0;
osg::ref_ptr<osg::MatrixTransform> ball1;
osg::ref_ptr<osg::Texture2D> texture;
osg::ref_ptr<osg::Camera> rttCamera;
osg::ref_ptr<osg::Camera> orthoCamera;

void createCircle()
{
  circle = new osg::Geometry;
  osg::ref_ptr<osg::Vec2Array> vertices = new osg::Vec2Array();
  osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array(osg::Array::BIND_OVERALL);
  
  circle->setVertexArray(vertices);
  circle->setColorArray(colors);

  GLuint slices = 64;
  GLfloat stepAngle = glm::f2pi/slices;

  vertices->reserve(slices + 2);
  vertices->push_back(osg::Vec2());
  for (int i = 0; i <= slices; ++i) {
    GLfloat angle = stepAngle * i;
    vertices->push_back(osg::Vec2(cos(angle), sin(angle)) * radius);
  }

  colors->push_back(osg::Vec4());

  circle->addPrimitiveSet(new osg::DrawArrays(GL_TRIANGLE_FAN, 0, vertices->size()));
  //osg::StateSet* ss = circle->getOrCreateStateSet();
}

void createRope()
{
  rope = new osg::Geometry;
  rope->setDataVariance(osg::Object::DYNAMIC);
  rope->setUseDisplayList(false);
  osg::ref_ptr<osg::Vec2Array> vertices = new osg::Vec2Array();
  osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array(osg::Array::BIND_OVERALL);
  
  rope->setVertexArray(vertices);
  rope->setColorArray(colors);
  vertices->resize(3);
  colors->push_back(osg::Vec4());
  
  rope->addPrimitiveSet(new osg::DrawArrays(GL_LINE_STRIP, 0, vertices->size()));
}

void createTrace()
{
  trace = new osg::Geometry;
  trace->setDataVariance(osg::Object::DYNAMIC);
  trace->setUseDisplayList(false);
  osg::ref_ptr<osg::Vec2Array> vertices = new osg::Vec2Array();
  osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array(osg::Array::BIND_OVERALL);
  
  trace->setVertexArray(vertices);
  trace->setColorArray(colors);
  vertices->resize(2);
  colors->push_back(osg::Vec4());
  
  trace->addPrimitiveSet(new osg::DrawArrays(GL_LINES, 0, vertices->size()));
}


void updatePendulum()
{
  last_pos = pos2;
  pos1 = origin + osg::Vec2(sin(theta1), -cos(theta1)) * l1;
  pos2 = pos1 + osg::Vec2(sin(theta2), -cos(theta2)) * l2;

  av1 += aa1;
  av2 += aa2;

  theta1 += av1;
  theta2 += av2;

  float denominator = 2*m1 + m2 - m2 * cos(2*theta1 - 2*theta2); 
  aa1= -g*(2*m1 + m2)*sin(theta1) - m2*g*sin(theta1 - 2*theta2) -
  2*sin(theta1 - theta2)*m2*(av2*av2*l2 + av1*av1*l1*cos(theta1 - theta2));
  aa2 = 2*sin(theta1 - theta2)*(av1*av1*l1*(m1 + m2) + g*(m1 + m2)*cos(theta1)
      + av2*av2*l2*m2*cos(theta1 - theta2));
  aa1 /= (l1*denominator);
  aa2 /= (l2*denominator);

  ball0->setMatrix(osg::Matrix::translate(osg::Vec3(pos1, 0)));
  ball1->setMatrix(osg::Matrix::translate(osg::Vec3(pos2, 0)));

  {
    osg::Vec2Array* vertices = static_cast<osg::Vec2Array*>(rope->getVertexArray());
    vertices->at(0) = origin;
    vertices->at(1) = pos1;
    vertices->at(2) = pos2;
    vertices->dirty();
    rope->dirtyBound();
  }

  {
    osg::Vec2Array* vertices = static_cast<osg::Vec2Array*>(trace->getVertexArray());
    vertices->at(0) = last_pos;
    vertices->at(1) = pos2;
    vertices->dirty();
    trace->dirtyBound();
  }
}

void createPendulum()
{
  createCircle();
  createRope();
  createTrace();

  //osg::StateSet* ss = rope->getOrCreateStateSet();

  line = new osg::MatrixTransform;
  {
    osg::ref_ptr<osg::Geode> leaf = new osg::Geode();
    line->addChild(leaf);
    leaf->addDrawable(rope);
  }

  ball0 = new osg::MatrixTransform;
  {
    osg::ref_ptr<osg::Geode> leaf = new osg::Geode();
    ball0->addChild(leaf);
    leaf->addDrawable(circle);
  }

  ball1 = new osg::MatrixTransform;
  {
    osg::ref_ptr<osg::Geode> leaf = new osg::Geode();
    ball1->addChild(leaf);
    leaf->addDrawable(circle);
  }

  {
  // use trace background
    osg::ref_ptr<osg::Geode> leaf = new osg::Geode();
    osg::Geometry* quad = osg::createTexturedQuadGeometry(osg::Vec3(-hext, 0),
        osg::Vec3(hext.x()*2, 0, 0), osg::Vec3(0, hext.y()*2, 0));
    leaf->addDrawable(quad);
    orthoRoot->addChild(leaf);
    osg::StateSet* ss = leaf->getOrCreateStateSet();
    ss->setTextureAttributeAndModes(0, texture);
  }

  orthoRoot->addChild(line);
  orthoRoot->addChild(ball0);
  orthoRoot->addChild(ball1);

  updatePendulum();

}

class RootUpdate : public osg::NodeCallback
{
  virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
  {
    if(clearRtt)
    {
      rttCamera->setClearMask(GL_COLOR_BUFFER_BIT);
      clearRtt = false;
    }
    else
      rttCamera->setClearMask(0);

    updatePendulum();
    traverse(node,nv);
  }
};

int main(int argc, char* argv[])
{
  root = new osg::Group;
  osg::StateSet* ss = root->getOrCreateStateSet();
  ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
  ss->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);

  root->setUpdateCallback(new RootUpdate);
  
  osgViewer::Viewer viewer;
  viewer.addEventHandler(new osgViewer::StatsHandler);
  viewer.setSceneData(root);
  viewer.setLightingMode(osg::View::NO_LIGHT);

  osg::GraphicsContext::WindowingSystemInterface* wsi = osg::GraphicsContext::getWindowingSystemInterface();
  wsi->getScreenResolution(osg::GraphicsContext::ScreenIdentifier(0), width, height);

  texture = new osg::Texture2D;
  texture->setResizeNonPowerOfTwoHint(false);
  texture->setTextureSize(width, height);
  texture->setInternalFormat(GL_RGBA);
  texture->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
  texture->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
  texture->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR);
  texture->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR);

  if(width >= height)
    hext.x() *= static_cast<GLfloat>(width)/height;
  else
    hext.y() *= static_cast<GLfloat>(height)/width;

  orthoCamera = new osg::Camera();
  root->addChild(orthoCamera);
  orthoCamera->setAllowEventFocus(false);
  orthoCamera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
  orthoCamera->setProjectionMatrixAsOrtho2D(-hext.x(), hext.x(), -hext.y(), hext.y());
  orthoCamera->setViewMatrix(osg::Matrix::identity());

  orthoRoot = new osg::Group;
  orthoCamera->addChild(orthoRoot);

  rttCamera = new osg::Camera();
  root->addChild(rttCamera);
  rttCamera->setRenderOrder(osg::Camera::PRE_RENDER);
  rttCamera->setAllowEventFocus(false);
  rttCamera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
  rttCamera->setProjectionMatrix(orthoCamera->getProjectionMatrix());
  rttCamera->setViewMatrix(orthoCamera->getViewMatrix());
  rttCamera->attach(osg::Camera::COLOR_BUFFER, texture);
  rttCamera->setRenderTargetImplementation(osg::Camera::FRAME_BUFFER_OBJECT);
  rttCamera->setViewport(0, 0, width, height);
  rttCamera->setComputeNearFarMode(osgUtil::CullVisitor::DO_NOT_COMPUTE_NEAR_FAR);

  rttRoot = new osg::Group;
  rttCamera->addChild(rttRoot);
  osg::ref_ptr<osg::Geode> traceLeaf = new osg::Geode();
  traceLeaf->setName("trace leaf");
  rttRoot->addChild(traceLeaf);
  
  osg::Camera* mainCamera = viewer.getCamera();
  // no clear needed, as trace will be used as background
  mainCamera->setClearMask(0);
  orthoCamera->setClearMask(0);
  rttCamera->setClearMask(0);

  createPendulum();
  traceLeaf->addDrawable(trace);

  osg::ref_ptr<osgUtil::PrintVisitor> pv = new osgUtil::PrintVisitor(osg::notify(osg::NOTICE));
  root->accept(*pv);

  return viewer.run();
}
