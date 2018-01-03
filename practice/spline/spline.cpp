#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osg/Geometry>
#include <osgDB/ReadFile>
#include <osg/MatrixTransform>
#include <osgWidget/WindowManager>
#include <osgWidget/Input>
#include "spline.h"
#include "stringutil.h"
#include <osgWidget/ViewerEventHandlers>
#include <osgText/String>
#include "zmath.h"
#include "common.h"
#include <osg/Point>
#include <osg/io_utils>
#include <iomanip>
#include <fstream>

class UiCallback;

osg::ref_ptr<osg::Group> root;
osg::ref_ptr<osg::Geode> leaf;
osg::ref_ptr<osg::Group> knotGroup;
osg::ref_ptr<zxd::Spline> spline, lc, rc;
osg::ref_ptr<osg::Geometry> iterations;
osg::Vec3Array* vertices;
osg::Camera* camera;
osg::Camera* hudCamera;
osg::Camera* uiCamera;
osg::ref_ptr<osgText::Font> font;
osg::ref_ptr<osgText::Text> text;
osg::Matrix matViewProjWnd, matInvViewProjWnd;

osgViewer::Viewer* viewer;

void updateIteratoins(bool);
void rebuildIterations();
void updateText();
void updateKnotsLable();
void readControlPoints();

bool playAnim = true;
bool subdivided = false;
GLdouble period = 5.0f;
GLdouble t = 0;  // normalized time
GLdouble ratio;  // ratio of project area and the entire screen. can be used to
                 // scale text
                 //
void readControlPoints() {
  osg::Vec3Array* controlPoints = spline->getControlPoints();
  controlPoints->clear();
  osg::DoubleArray* knots = spline->getKnots();
  knots->clear();

  // read control points and knots from spline.txt
  std::ifstream ifs("spline.txt");
  if (ifs.fail()) {
    OSG_FATAL << "failed to read open spline.txt" << std::endl;
    return;
  }

  std::string line;
  // read until "control points"
  while (std::getline(ifs, line)) {
    if (line.size() >= 14 && line.substr(0, 14) == "control points") break;
  }

  while (std::getline(ifs, line)) {
    if (line.size() >= 5 && line.substr(0, 5) == "knots") break;
    if (line.empty()) continue;

    std::stringstream ss(line);
    osg::Vec3 v;
    ss >> v[0];
    ss >> v[1];
    ss >> v[2];
    controlPoints->push_back(v);
    OSG_NOTICE << v << std::endl;
  }

  GLfloat knot;
  while (ifs >> knot) knots->push_back(knot);

  spline->updateDegree();
  OSG_NOTICE << "read " << controlPoints->size() << " control points, "
             << knots->size() << " knots " << std::endl;
}

void reset() {
  t = 0;
  subdivided = false;

  readControlPoints();
  // setup  hudcamera projection area according to the size of spline polyline
  const osg::BoundingBox& bb = spline->getPolylineBoundingBox();
  osg::Vec3 center = bb.center();
  GLdouble w = bb.xMax() - bb.xMin();
  GLdouble h = bb.yMax() - bb.yMin();
  GLdouble a = std::max(w, h) * 1.5;
  zxd::setHudCameraFocus(
    hudCamera, a, center.x(), center.y(), zxd::getScreenAspectRatio());

  GLuint width, height;
  zxd::getScreenResolution(width, height);
  // use default identity view matrix
  matViewProjWnd = hudCamera->getProjectionMatrix() *
                   zxd::Math::computeWindowMatrix(0, width, 0, height);

  matInvViewProjWnd = osg::Matrix::inverse(matViewProjWnd);

  leaf->removeDrawables(0, leaf->getNumDrawables());
  leaf->addDrawable(spline);

  spline->rebuild();

  rebuildIterations();
  updateKnotsLable();
  leaf->addDrawable(iterations);
}

// draw knot dot, index and it's value
void updateKnotsLable() {
  knotGroup->removeChildren(0, knotGroup->getNumChildren());

  static osg::ref_ptr<osg::Geometry> dot =
    zxd::createSingleDot(4.0f, osg::Vec4(1.0f, 0.0f, 1.0f, 1.0f));

  osg::DoubleArray* knots = spline->getKnots();
  for (unsigned int i = 0; i < knots->size(); ++i) {
    GLfloat u = knots->at(i);
    // get window position as position for this knot. As uicamera is window
    // aligned
    osg::Vec3 p = spline->get(u) * matViewProjWnd;  
    p.z() = 0;
    osg::ref_ptr<osg::MatrixTransform> mt = new osg::MatrixTransform();
    mt->setMatrix(osg::Matrix::translate(p));
    osg::ref_ptr<osg::Geode> l = new osg::Geode();
    // add dot, and text
    l->addDrawable(dot);
    std::stringstream ss;
    ss << i << " : " << std::fixed << std::setprecision(3) << u;
    l->addDrawable(zxd::createText(font, osg::Vec3(0, 5, 0), ss.str(), 12));

    mt->addChild(l);
    knotGroup->addChild(mt);
  }
}

void subdivide() {
  subdivided = true;
  spline->subdivide(t, *lc, *rc);
  lc->setDegree(spline->getDegree());
  rc->setDegree(spline->getDegree());
  lc->rebuild();
  rc->rebuild();
  leaf->removeDrawables(0, leaf->getNumDrawables());
  leaf->addDrawable(lc);
  leaf->addDrawable(rc);
  knotGroup->removeChildren(0, knotGroup->getNumChildren());
}

class SplineAnimCallback : public osg::NodeCallback {
  virtual void operator()(osg::Node* node, osg::NodeVisitor* nv) {
    // becareful here, there is only 1 frame stamp in nv, it's content keeps
    // changing, dont try to store entire old framestamp

    static GLfloat t0 = nv->getFrameStamp()->getReferenceTime();
    if (!playAnim || subdivided) {
      t0 = nv->getFrameStamp()->getReferenceTime();
      return;
    }

    GLfloat dt = nv->getFrameStamp()->getReferenceTime() - t0;
    t += dt / period;
    if (t > 1.0f) t -= 1.0f;
    updateIteratoins(false);
    updateText();

    t0 = nv->getFrameStamp()->getReferenceTime();
  }
};

// control points grab handler
class BezierController : public osgGA::GUIEventHandler {
protected:
  bool mGrabing;
  osg::Vec3* mPoint;  // current grabing point
  osg::Matrix mInvViewProjWnd;
  osg::Vec3 mWndPos;     // start window position with depth value
  osg::Vec2 mCursorPos;  // start cursor position
  osg::ref_ptr<osg::Geometry> mGmPoint;
  osg::ref_ptr<osg::MatrixTransform> mPointNode;

public:
  BezierController() : mGrabing(false), mPoint(0) {}

protected:
  virtual bool handle(
    const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) {
    switch (ea.getEventType()) {
      case osgGA::GUIEventAdapter::PUSH:
        if (ea.getButton() & osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
          selectPoint(ea);
        break;
      case osgGA::GUIEventAdapter::RELEASE:
        mPoint = 0;
        if (mPointNode) mPointNode->setNodeMask(0);

        break;

      case osgGA::GUIEventAdapter::DRAG: {
        if (!mPoint) break;

        osg::Vec2 offset = osg::Vec2(ea.getX(), ea.getY()) - mCursorPos;
        osg::Vec3 targetWndPos = mWndPos + osg::Vec3(offset, 0);
        *mPoint = targetWndPos * matInvViewProjWnd;

        spline->rebuild();
        updateIteratoins(false);
        updateKnotsLable();

        if (mPointNode) mPointNode->setMatrix(osg::Matrix::translate(*mPoint));

      } break;

      case osgGA::GUIEventAdapter::KEYDOWN: {
        switch (ea.getKey()) {
          case osgGA::GUIEventAdapter::KEY_Left:
            if (!playAnim && !subdivided) {
              t -= 0.05;
              t = std::max(0.0, t);
              updateIteratoins(false);
              updateText();
            }
            break;
          case osgGA::GUIEventAdapter::KEY_Right:
            if (!playAnim && !subdivided) {
              t += 0.05;
              t = std::min(1.0, t);
              updateIteratoins(false);
              updateText();
            }
            break;
          case 'a':
            playAnim = !playAnim;
            break;
          case 'd':
            if (!subdivided) subdivide();
            break;
          case 'r':
            reset();
          default:
            break;
        }
      }

      default:
        break;
    }
    return false;  // return true will stop event
  }

  void selectPoint(const osgGA::GUIEventAdapter& ea) {
    float threashold = 50.0f;
    osg::Vec3Array* controlPoints = spline->getControlPoints();

    for (unsigned int i = 0; i < controlPoints->size(); ++i) {
      osg::Vec3 wndPos = controlPoints->at(i) * matViewProjWnd;
      osg::Vec2 offset =
        osg::Vec2(wndPos.x() - ea.getX(), wndPos.y() - ea.getY());
      if (offset.length2() <= threashold) {
        mPoint = &controlPoints->at(i);
        mWndPos = wndPos;
        break;
      }
    }

    if (mPoint) {
      OSG_NOTICE << "select control point " << *mPoint << std::endl;
      mCursorPos = osg::Vec2(ea.getX(), ea.getY());

      if (!mGmPoint) {
        mGmPoint = new osg::Geometry;
        osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array();
        osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array();
        colors->setBinding(osg::Array::BIND_OVERALL);

        vertices->push_back(osg::Vec3());
        colors->push_back(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));

        mGmPoint->setVertexArray(vertices);
        mGmPoint->setColorArray(colors);

        mGmPoint->addPrimitiveSet(new osg::DrawArrays(GL_POINTS, 0, 1));

        osg::StateSet* ss = mGmPoint->getOrCreateStateSet();
        ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
        ss->setAttributeAndModes(new osg::Point(15.0f));

        mPointNode = new osg::MatrixTransform();
        osg::ref_ptr<osg::Geode> l = new osg::Geode();
        l->addDrawable(mGmPoint);
        mPointNode->addChild(l);

        hudCamera->addChild(mPointNode);
      }

      mPointNode->setNodeMask(-1);
      mPointNode->setMatrix(osg::Matrix::translate(*mPoint));
    }
  }
};

void createScene() {
  root = new osg::Group;
  leaf = new osg::Geode();
  knotGroup = new osg::Group;
  hudCamera = zxd::createHudCamera();
  // need to draw single point
  hudCamera->setCullingMode(
    hudCamera->getCullingMode() & ~osg::CullSettings::SMALL_FEATURE_CULLING);
  uiCamera = zxd::createHudCamera();

  spline = new zxd::Spline;
  spline->setUseDisplayList(false);
  lc = new zxd::Spline;  // subdivided left curve
  rc = new zxd::Spline;  // subdivided right curve

  {
    osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array();
    colors->push_back(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));
    colors->setBinding(osg::Array::Binding::BIND_OVERALL);
    spline->setColorArray(colors);
  }
  {
    osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array();
    colors->push_back(osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f));
    colors->setBinding(osg::Array::Binding::BIND_OVERALL);
    lc->setColorArray(colors);
  }
  {
    osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array();
    colors->push_back(osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f));
    colors->setBinding(osg::Array::Binding::BIND_OVERALL);
    rc->setColorArray(colors);
  }

  osg::StateSet* ss = leaf->getOrCreateStateSet();
  ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

  // animate iterations
  spline->addUpdateCallback(new SplineAnimCallback());

  root->addChild(hudCamera);
  uiCamera->addChild(knotGroup);
  root->addChild(uiCamera);
  hudCamera->addChild(leaf);

  reset();
}

osgWidget::Input* createInput(
  const std::string& name, const std::string& text, int size) {
  osgWidget::Input* input = new osgWidget::Input(name, text, size);
  input->setFont("fonts/VeraMono.ttf");
  input->setFontColor(0.0f, 0.0f, 0.0f, 1.0f);
  input->setFontSize(15);
  input->setYOffset(input->calculateBestYOffset("y"));  //?
  input->setSize(100.0f, input->getText()->getCharacterHeight());
  input->unfocus(0);

  return input;
}

osgWidget::Label* createLabel(
  const std::string& name, const std::string& text) {
  osgWidget::Label* label = new osgWidget::Label(name, text);
  label->setFont("fonts/VeraMono.ttf");
  label->setFontColor(0.0f, 0.0f, 0.0f, 1.0f);
  label->setFontSize(15);
  label->setSize(70.0f, label->getText()->getCharacterHeight());

  return label;
}

void updateIteratoins(bool updatePrimitiveSet = false) {
  // sometimes t is negative,  cuased by computer double precision problem?
  GLdouble nt = std::min(std::max(0.0, t), 1.0);
  GLdouble u = spline->getMinKnot() * (1 - nt) + spline->getMaxKnot() * nt;

  GLuint k = spline->getKnotSpan(u);  // knot span
  GLuint p = spline->getDegree();

  // find iterations for from k-p to p, don't care about multiplicity
  zxd::Vec3ArrayVec vec = spline->iterate(k - p, k + 1, u, p);
  osg::Vec3Array* points = spline->getControlPoints();

  if (updatePrimitiveSet) {
    iterations->removePrimitiveSet(0, iterations->getNumPrimitiveSets());

    // reset color for every vertex in iterations
    osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array();
    colors->setBinding(osg::Array::Binding::BIND_PER_VERTEX);
    iterations->setColorArray(colors);

    int numIterations = vec.size();
    // colors for 1st iteration
    colors->insert(colors->end(), points->size(), zxd::Math::randomRGB4());

    for (int i = 1; i < numIterations; ++i) {
      colors->insert(colors->end(), numIterations - i,
        osg::Vec4(zxd::Math::randomVector(0.0f, 1.0f), 1.0f));
    }
  }

  osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array();
  iterations->setVertexArray(vertices);
  vertices->reserve(
    (points->size() + 1) * points->size() / 2);  // more than enough

  // vertices for 1st iteration
  vertices->assign(points->begin(), points->end());
  if (updatePrimitiveSet)
    iterations->addPrimitiveSet(
      new osg::DrawArrays(GL_LINE_STRIP, 0, vertices->size()));

  // vertices for other iterations
  for (unsigned int i = 1; i < vec.size(); ++i) {
    osg::Vec3Array* va = vec[i];
    vertices->insert(vertices->end(), va->begin(), va->end());
    if (updatePrimitiveSet)
      iterations->addPrimitiveSet(
        new osg::DrawArrays(i == vec.size() - 1 ? GL_POINTS : GL_LINE_STRIP,
          vertices->size() - va->size(), va->size()));
  }

  // draw all points
  if (updatePrimitiveSet)
    iterations->addPrimitiveSet(
      new osg::DrawArrays(GL_POINTS, 0, vertices->size()));
}

void rebuildIterations() {
  if (!iterations) {
    iterations = new osg::Geometry;
    osg::StateSet* ss = iterations->getOrCreateStateSet();
    ss->setAttributeAndModes(new osg::Point(8.0f));
  }
  updateIteratoins(true);
}

void updateText() {
  if (!text) {
    GLuint width, height;
    zxd::getScreenResolution(width, height);

    font = osgText::readFontFile("fonts/arials.ttf");

    text = zxd::createText(font, osg::Vec3(10.0f, height - 30, 0.0f), "", 12);
    osg::ref_ptr<osg::Geode> leaf = new osg::Geode();
    leaf->addDrawable(text);
    uiCamera->addChild(leaf);
  }

  std::stringstream ss;
  ss << "  r : reset \n";
  ss << "  left|right arrow : change t if animation stopped \n";
  ss << "  a : pause animation \n";
  ss << "  d : subdivide. (u can only reset after this operation) \n";
  ss << "  drag : move control points \n";
  ss << "  degree : " << spline->getDegree() << std::endl;
  ss << "  n p m : " << spline->n() << " " << spline->p() << " " << spline->m()
     << std::endl;
  ss << "  t : " << t << std::endl;
  text->setText(ss.str());
}

int main(int argc, char* argv[]) {
  osgViewer::Viewer v;
  viewer = &v;
  camera = viewer->getCamera();

  createScene();
  updateText();

  v.addEventHandler(new osgViewer::StatsHandler);
  v.addEventHandler(new BezierController());
  v.setSceneData(root);

  // no camera manipulator
  while (!v.done()) v.frame();
}
