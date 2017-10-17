#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osg/Geometry>
#include <osgDB/ReadFile>
#include <osg/MatrixTransform>
#include <osgWidget/WindowManager>
#include <osgWidget/Input>
#include "bezier.h"
#include "stringutil.h"
#include <osgWidget/ViewerEventHandlers>
#include <osgText/String>
#include "zmath.h"
#include "common.h"
#include <osg/Point>
#include <osg/io_utils>

class UiCallback;

osg::ref_ptr<osg::Group> root;
osg::ref_ptr<osg::Geode> leaf;
osg::ref_ptr<zxd::Bezier> bezier, lc, rc;
osg::ref_ptr<osg::Geometry> iterations;
osg::Vec3Array* vertices;
osg::Camera* hudCamera;
osg::Camera* uiCamera;
osg::ref_ptr<osgText::Font> font;
osg::ref_ptr<osgText::Text> text;

osgViewer::Viewer* viewer;

void updateIteratoins(bool);
void rebuildIterations();
void updateText();

bool playAnim = true;
bool subdivided = false;
float period = 3.0f;
float t;

void reset() {
  t = 0;
  subdivided = false;

  leaf->removeDrawables(0, leaf->getNumDrawables());
  leaf->addDrawable(bezier);

  // reset control points
  GLuint width, height;
  zxd::getScreenResolution(width, height);

  osg::Vec3Array* controlPoints = bezier->getControlPoints();
  controlPoints->clear();
  controlPoints->push_back(osg::Vec3(width * 0.25f, height * 0.25f, 0));
  controlPoints->push_back(osg::Vec3(width * 0.35f, height * 0.75f, 0));
  controlPoints->push_back(osg::Vec3(width * 0.65f, height * 0.75f, 0));
  controlPoints->push_back(osg::Vec3(width * 0.75f, height * 0.25f, 0));

  bezier->rebuild();
  rebuildIterations();
  leaf->addDrawable(iterations);
}

void subdivide() {
  subdivided = true;
  bezier->subdivide(t, lc, rc);
  lc->rebuild();
  rc->rebuild();
  leaf->removeDrawables(0, leaf->getNumDrawables());
  leaf->addDrawable(lc);
  leaf->addDrawable(rc);
}

class BezierAnimCallback : public osg::NodeCallback {
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
        *mPoint = targetWndPos;

        bezier->rebuild();
        updateIteratoins(false);

        if (mPointNode)
          mPointNode->setMatrix(osg::Matrix::translate(targetWndPos));

      } break;

      case osgGA::GUIEventAdapter::KEYDOWN: {
        switch (ea.getKey()) {
          case osgGA::GUIEventAdapter::KEY_Up:
            bezier->elevate();
            bezier->rebuild();
            updateIteratoins(true);
            updateText();
            break;
          case osgGA::GUIEventAdapter::KEY_Down:
            bezier->elevate(false);
            bezier->rebuild();
            updateIteratoins(true);
            updateText();
            break;
          case osgGA::GUIEventAdapter::KEY_Left:
            if (!playAnim && !subdivided) {
              t -= 0.05f;
              t = std::max(0.0f, t);
              updateIteratoins(false);
              updateText();
            }
            break;
          case osgGA::GUIEventAdapter::KEY_Right:
            if (!playAnim && !subdivided) {
              t += 0.05f;
              t = std::min(1.0f, t);
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
    osg::Vec3Array* controlPoints = bezier->getControlPoints();

    for (unsigned int i = 0; i < controlPoints->size(); ++i) {
      osg::Vec3 wndPos = controlPoints->at(i);
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
        osg::ref_ptr<osg::Geode> leaf = new osg::Geode();
        leaf->addDrawable(mGmPoint);
        mPointNode->addChild(leaf);

        hudCamera->addChild(mPointNode);
      }

      mPointNode->setNodeMask(-1);
      mPointNode->setMatrix(osg::Matrix::translate(*mPoint));
    }
  }
};

void createScene() {
  hudCamera = zxd::createHUDCamera();
  // need to draw single point
  hudCamera->setCullingMode(
    hudCamera->getCullingMode() & ~osg::CullSettings::SMALL_FEATURE_CULLING);

  root = new osg::Group;
  leaf = new osg::Geode();
  root->addChild(hudCamera);
  hudCamera->addChild(leaf);

  bezier = new zxd::Bezier;
  bezier->setUseDisplayList(false);
  lc = new zxd::Bezier;  // subdivided left curve
  rc = new zxd::Bezier;  // subdivided right curve

  osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array();
  colors->push_back(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));
  colors->setBinding(osg::Array::Binding::BIND_OVERALL);
  bezier->setColorArray(colors);

  osg::StateSet* ss = leaf->getOrCreateStateSet();
  ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

  // animate iterations
  bezier->addUpdateCallback(new BezierAnimCallback());

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
  if (updatePrimitiveSet) {
    iterations->removePrimitiveSet(0, iterations->getNumPrimitiveSets());

    // reset color for every vertex in iterations
    osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array();
    colors->setBinding(osg::Array::Binding::BIND_PER_VERTEX);
    iterations->setColorArray(colors);

    int numIterations = bezier->getDegree();
    for (int i = 0; i < numIterations; ++i) {
      colors->insert(colors->end(), numIterations - i + 1,
        osg::Vec4(zxd::Math::randomVector(0.0f, 1.0f), 1.0f));
    }
  }

  osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array();
  iterations->setVertexArray(vertices);

  zxd::Vec3VecVec vv = zxd::Bezier::iterateAll(*bezier->getControlPoints(), t);
  // draw all iterations, except the last one, which is a single point
  for (unsigned int i = 0; i < vv.size(); ++i) {
    osg::Vec3Array* va = vv[i];
    vertices->insert(vertices->end(), va->begin(), va->end());
    if (updatePrimitiveSet)
      iterations->addPrimitiveSet(
        new osg::DrawArrays(i == vv.size() - 1 ? GL_POINTS : GL_LINE_STRIP,
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
    hudCamera->addChild(leaf);
  }

  std::stringstream ss;
  ss << "  r : reset \n";
  ss << "  up|down arrow : change degree \n";
  ss << "  left|right arrow : change t if animation stopped \n";
  ss << "  a : pause animation \n";
  ss << "  d : subdivide. (u can only reset after this operation) \n";
  ss << "  drag : move control points \n";
  ss << "  degree : " << bezier->n() << std::endl;
  ss << "  t : " << t << std::endl;
  text->setText(ss.str());
}

int main(int argc, char* argv[]) {
  osgViewer::Viewer v;
  viewer = &v;

  createScene();
  updateText();

  v.addEventHandler(new osgViewer::StatsHandler);
  v.addEventHandler(new BezierController());
  v.setSceneData(root);

  // no camera manipulator
  while (!v.done()) v.frame();
}
