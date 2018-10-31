#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgUtil/PrintVisitor>
#include <osgUtil/SmoothingVisitor>
#include <osgText/Text>
#include <osgDB/ReadFile>
#include <cassert>
#include <sstream>
#include "glm.h"
#include "geometryutil.h"

osg::ref_ptr<osg::Group> root;
osg::ref_ptr<osg::Geometry> knot;
osg::ref_ptr<osg::Geode> knotLeaf;
osg::ref_ptr<osgText::Text> text;
GLuint slices = 1000;
GLuint extrudeRadius = 5;
GLint numFaces = 6;
GLuint knotIndex = 0;
GLuint numKnots = 2;

osg::Vec3 rptBeta4(GLfloat beta, GLfloat radius)
{
  GLfloat r = 0.8 + 1.6 * sin(6 * beta);
  r *= radius;
  GLfloat theta = 2 * beta;
  GLfloat phi = 0.6 * glm::fpi * sin(12 * beta);
  return osg::Vec3(r, phi, theta);
}

osg::Vec3 rptBeta5(GLfloat beta, GLfloat radius)
{
  GLfloat r = 1.2 * 0.6 * sin(0.5 * glm::fpi + 6 * beta);
  r *= radius;
  GLfloat theta = 4 * beta;
  GLfloat phi = 0.2 * glm::fpi * sin(6 * beta);
  return osg::Vec3(r, phi, theta);
}

osg::Vec3 rptToXyz(GLfloat r, GLfloat phi, GLfloat theta)
{
  GLfloat x = r * cos(phi) * cos(theta);
  GLfloat y = r * cos(phi) * sin(theta);
  GLfloat z = r * sin(phi);
  return osg::Vec3(x, y, z);
}

void update_geometry(const osg::Vec3Array* lineStrip)
{
  osg::ref_ptr<osg::Vec3Array> vertices = 
    geometryUtil::extrude_along_line_strip(lineStrip, extrudeRadius, numFaces);

  knot->setVertexArray(vertices);
  knot->removePrimitiveSet(0, knot->getNumPrimitiveSets());
  knot->addPrimitiveSet(new osg::DrawArrays(GL_TRIANGLES, 0, vertices->size()));
  knot->dirtyBound();
  knot->dirtyDisplayList();
  vertices->dirty();

  osgUtil::SmoothingVisitor visitor;
  knotLeaf->accept(visitor);
}

void buildRptKnot(std::function<osg::Vec3 (float, float)> rpt_func)
{
  GLfloat radius = 100;

  GLfloat step_beta = glm::fpi / slices;

  osg::ref_ptr<osg::Vec3Array> line_strip = new osg::Vec3Array;
  line_strip->reserve(slices + 1);
  for (int i = 0; i < slices; ++i) {
    GLfloat beta = step_beta * i;
    osg::Vec3 rpt = rpt_func(beta, radius);
    line_strip->push_back(rptToXyz(rpt.x(), rpt.y(), rpt.z()));
  }
  line_strip->push_back(line_strip->front());
  update_geometry(line_strip);
}

void resetKnot()
{
  if(knotIndex == 0)
  {
    buildRptKnot(rptBeta4);
  }
  else if(knotIndex == 1)
  {
    buildRptKnot(rptBeta5);
  }

  std::stringstream ssm;
  ssm << "q : knot : " << knotIndex << std::endl;
  ssm << "wW : face : " << numFaces << std::endl;
  text->setText(ssm.str());
}

class KnotInputHandler : public osgGA::GUIEventHandler
{
  virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
  {
    bool shiftDown = ea.getModKeyMask() & osgGA::GUIEventAdapter::MODKEY_SHIFT;
    switch (ea.getEventType())
    {
      case osgGA::GUIEventAdapter::KEYDOWN:
        switch (ea.getUnmodifiedKey())
        {
          case osgGA::GUIEventAdapter::KEY_Q:
            knotIndex = ++knotIndex % numKnots;
            resetKnot();
            break;
          case osgGA::GUIEventAdapter::KEY_W:
            numFaces += shiftDown ? -1 : 1;
            numFaces = osg::clampTo(numFaces, 3, 1000);
            resetKnot();

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
  root = new osg::Group;
  osg::StateSet* ss = root->getOrCreateStateSet();
  //ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
  ss->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
  ss->setMode(GL_CULL_FACE, osg::StateAttribute::ON);

  knotLeaf = new osg::Geode;
  root->addChild(knotLeaf);
  knot = new osg::Geometry;
  knot->setDataVariance(osg::Object::DYNAMIC);
  knotLeaf->addDrawable(knot);


  osgViewer::Viewer viewer;
  viewer.addEventHandler(new osgViewer::StatsHandler);
  viewer.addEventHandler(new KnotInputHandler);
  viewer.setSceneData(root);

  GLuint width, height;
  osg::GraphicsContext::WindowingSystemInterface* wsi = osg::GraphicsContext::getWindowingSystemInterface();
  assert(wsi);
  wsi->getScreenResolution(osg::GraphicsContext::ScreenIdentifier(0), width, height);

  osg::ref_ptr<osg::Camera> hudCamera = new osg::Camera();
  root->addChild(hudCamera);
  hudCamera->setAllowEventFocus(false);
  hudCamera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
  hudCamera->setClearMask(0);
  hudCamera->setRenderOrder(osg::Camera::POST_RENDER);
  hudCamera->setProjectionMatrixAsOrtho(0, width, 0, height, -1, 1);
  hudCamera->setViewMatrix(osg::Matrix::identity());
  hudCamera->setComputeNearFarMode(osgUtil::CullVisitor::DO_NOT_COMPUTE_NEAR_FAR);
  {
    osg::StateSet* ss = hudCamera->getOrCreateStateSet();
    ss->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
    ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
  }

  text = new osgText::Text;
  text->setDataVariance(osg::Object::DYNAMIC);
  text->setCharacterSize(20);
  text->setAxisAlignment(osgText::TextBase::XY_PLANE);
  text->setPosition(osg::Vec3(20, height - 30, 0));
  text->setFont(osgText::readFontFile("fonts/arial.ttf"));

  osg::ref_ptr<osg::Geode> textLeaf = new osg::Geode();
  hudCamera->addChild(textLeaf);
  textLeaf->addDrawable(text);

  resetKnot();

  osg::ref_ptr<osgUtil::PrintVisitor> pv = new osgUtil::PrintVisitor(osg::notify(osg::NOTICE));
  root->accept(*pv);


  return viewer.run();
}
