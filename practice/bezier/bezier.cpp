/*
 * Create bezier curve, play bezier animation. This bezire will be drawn
 * on xy plane.
 *
 * You can change following items to generate different kinds of bezier curve:
 *
 *      degree : 1-5 // i don't think i will ever use 5+
 *      control points, grab like blender
 *      animation period
 *      animation toggle
 *      interpolate time
 *      segments
 */

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

osg::ref_ptr<osg::Vec3Array> controlPoints;
osg::ref_ptr<osg::Group> root;
osg::ref_ptr<osg::Geometry> gmCurve;
osg::ref_ptr<osg::Program> program;
osg::ref_ptr<osg::Uniform> uniform;
osg::ref_ptr<zxd::Bezier> bezier;
osg::ref_ptr<osg::Geometry> intermediateLines;
osg::Vec3Array* vertices;
osg::Camera* hudCamera;
osg::Camera* uiCamera;
osg::Camera* camera;  // main camera
osg::ref_ptr<UiCallback> uiCallback;

osgViewer::Viewer* viewer;
osgWidget::Input* editDegree;
osgWidget::Input* editSegments;
osgWidget::Input* editPlayAnim;
osgWidget::Input* editPeriod;
osgWidget::Input* editTime;

void updateIntermediateLinePoints(bool);
void rebuildIntermediateLines();

class BezierAnimCallback : public osg::NodeCallback {
  virtual void operator()(osg::Node* node, osg::NodeVisitor* nv) {
    double t = nv->getFrameStamp()->getReferenceTime();
    t = std::fmod(t, 3.0f) / 3.0f;
    if (editTime) {
      editTime->setLabel(zxd::StringUtil::toString(t));
    }
    updateIntermediateLinePoints(false);
  }
};

// control points grab handler
class GrabHandler : public osgGA::GUIEventHandler {
protected:
  bool mGrabing;
  osg::Vec3* mPoint;  // current grabing point
  osg::Matrix mInvViewProjWnd;
  osg::Vec3 mWndPos;     // start window position with depth value
  osg::Vec2 mCursorPos;  // start cursor position

public:
  GrabHandler() : mGrabing(false), mPoint(0) {}

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
        break;

      case osgGA::GUIEventAdapter::DRAG: {
        if (!mPoint) break;

        osg::Vec2 offset = osg::Vec2(ea.getX(), ea.getY()) - mCursorPos;
        osg::Vec3 targetWndPos = mWndPos + osg::Vec3(offset, 0);
        *mPoint = targetWndPos;

        bezier->rebuild();
        updateIntermediateLinePoints(false);

      } break;
      default:
        break;
    }
    return false;  // return true will stop event
  }

  void selectPoint(const osgGA::GUIEventAdapter& ea) {
    float threashold = 50.0f;

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
    }
  }
};

void createScene() {
  hudCamera = zxd::createHUDCamera();

  root = new osg::Group;
  bezier = new zxd::Bezier;
  bezier->setUseDisplayList(false);
  bezier->osg::Drawable::setUseVertexBufferObjects(true);

  osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array();
  colors->push_back(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));
  colors->setBinding(osg::Array::Binding::BIND_OVERALL);
  bezier->setColorArray(colors);

  controlPoints = new osg::Vec3Array();
  bezier->setControlPoints(controlPoints);

  GLuint width, height;
  zxd::getScreenResolution(width, height);
  controlPoints->push_back(osg::Vec3(width * 0.25f, height * 0.5f, 0));
  controlPoints->push_back(osg::Vec3(width * 0.35f, height * 0.75f, 0));
  controlPoints->push_back(osg::Vec3(width * 0.65f, height * 0.75f, 0));
  controlPoints->push_back(osg::Vec3(width * 0.75f, height * 0.5f, 0));

  bezier->rebuild();
  rebuildIntermediateLines();

  osg::ref_ptr<osg::Geode> leaf = new osg::Geode();
  leaf->addDrawable(bezier);
  bezier->addUpdateCallback(new BezierAnimCallback());

  osg::StateSet* ss = leaf->getOrCreateStateSet();
  ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

  hudCamera->addChild(leaf);

  root->addChild(hudCamera);
}

osgWidget::Input* createInput(
  const std::string& name, const std::string& text, int size) {
  osgWidget::Input* input = new osgWidget::Input(name, text, size);
  input->setFont("fonts/VeraMono.ttf");
  input->setFontColor(0.0f, 0.0f, 0.0f, 1.0f);
  input->setFontSize(15);
  input->setYOffset(input->calculateBestYOffset("y"));  //?
  input->setSize(100.0f, input->getText()->getCharacterHeight());
  return input;
}

void updateIntermediateLinePoints(bool updatePrimitiveSet = false) {
  if (updatePrimitiveSet)
    intermediateLines->removePrimitiveSet(
      0, intermediateLines->getNumPrimitiveSets());

  osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array();
  intermediateLines->setVertexArray(vertices);

  int numLineGroups = controlPoints->size() - 1;
  osg::ref_ptr<osg::Vec3Array> va = controlPoints;

  float t = 0;
  if (editTime) t = zxd::StringUtil::parseFloat(editTime->getLabel());

  for (int i = 0; i < numLineGroups; ++i) {
    // std::for_each(va->begin(), va->end(),
    //[&](decltype(*va->begin()) v) { OSG_NOTICE << v << std::endl; });

    vertices->insert(vertices->end(), va->begin(), va->end());
    if (updatePrimitiveSet)
      intermediateLines->addPrimitiveSet(new osg::DrawArrays(
        GL_LINE_STRIP, vertices->size() - va->size(), va->size()));
    va = zxd::Bezier::getIntermediatePoints(va, t);
  }

  // add current interpolate point
  vertices->push_back(va->at(0));

  // draw all points
  if (updatePrimitiveSet)
    intermediateLines->addPrimitiveSet(
      new osg::DrawArrays(GL_POINTS, 0, vertices->size()));
}

void rebuildIntermediateLines() {
  static osg::ref_ptr<osg::Vec4Array> scolors = new osg::Vec4Array();
  if (scolors->empty()) {
    scolors->push_back(osg::Vec4(0.5f, 0.5f, 0.5f, 1.0f));
    scolors->push_back(osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f));
    scolors->push_back(osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f));
    scolors->push_back(osg::Vec4(0.5f, 0.0f, 0.8f, 1.0f));
    scolors->push_back(osg::Vec4(0.7f, 0.0f, 0.5f, 1.0f));
    scolors->setBinding(osg::Array::Binding::BIND_PER_VERTEX);
  }

  if (!intermediateLines) {
    intermediateLines = new osg::Geometry;
    osg::StateSet* ss = intermediateLines->getOrCreateStateSet();
    ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
    ss->setAttributeAndModes(new osg::Point(8.0f));

    osg::ref_ptr<osg::Geode> leaf = new osg::Geode();
    leaf->addDrawable(intermediateLines);
    hudCamera->addChild(leaf);
  }
  osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array();
  colors->setBinding(osg::Array::Binding::BIND_PER_VERTEX);
  intermediateLines->setColorArray(colors);

  int numLineGroups = controlPoints->size() - 1;
  for (int i = 0; i < numLineGroups; ++i) {
    // colors->push_back(scolors->at(i));
    colors->insert(colors->end(), numLineGroups - i + 1, scolors->at(i));
  }
  colors->push_back(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));

  updateIntermediateLinePoints(true);
}

// union of input change call back
class UiCallback : public osg::Referenced {
public:
  bool onDegreeChange(osgWidget::Event& e) {
    int degree = zxd::StringUtil::parseInt(editDegree->getLabel(), 3);
    degree = std::max(std::min(degree, 5), 0);
    // editDegree->getText()->setText(zxd::StringUtil::toString(degree));

    int curDegree = bezier->getDegree();
    int difference = degree - curDegree;

    if (difference > 0) {
      // insert new vertices at the end or last second
      osg::Vec3Array::iterator iter =
        curDegree <= 1 ? controlPoints->end() : controlPoints->end() - 1;
      controlPoints->insert(iter, difference, osg::Vec3());
    } else if (difference < 0) {
      // remove  controlPoints at the end or last second
      osg::Vec3Array::iterator iter =
        curDegree == 1 ? controlPoints->end() - 1 : controlPoints->end() - 2;
      controlPoints->erase(iter, iter - difference);
    }

    bezier->rebuild();
    return false;
  }
};

void createUi() {
  // what's this?
  const unsigned int MASK_2D = 0xF0000000;

  GLuint width, height;
  zxd::getScreenResolution(width, height);

  osg::ref_ptr<osgWidget::WindowManager> wm = new osgWidget::WindowManager(
    viewer, width, height, MASK_2D, osgWidget::WindowManager::WM_PICK_DEBUG);

  osgWidget::Box* box = new osgWidget::Box("vbox", osgWidget::Box::VERTICAL);

  editDegree = createInput("degree", "3", 20);
  editSegments = createInput("segments", "20", 20);
  editPlayAnim = createInput("playAnim", "1", 20);
  editPeriod = createInput("period", "2", 20);
  editTime = createInput("time", "0", 20);
  uiCallback = new UiCallback;

  // editDegree->addCallback(new osgWidget::Callback(
  //&UiCallback::onDegreeChange, uiCallback.get(), osgWidget::EVENT_KEY_DOWN));

  box->addWidget(editDegree);
  box->addWidget(editSegments);
  box->addWidget(editPlayAnim);
  box->addWidget(editPeriod);
  box->addWidget(editTime);

  box->setOrigin(200.0f, 200);
  wm->addChild(box);

  uiCamera = wm->createParentOrthoCamera();

  //viewer->addEventHandler(new osgWidget::MouseHandler(wm));
  //viewer->addEventHandler(new osgWidget::KeyboardHandler(wm));
  viewer->addEventHandler(new osgWidget::ResizeHandler(wm, uiCamera));
  // viewer->addEventHandler(new osgWidget::CameraSwitchHandler(wm, uiCamera));
  viewer->addEventHandler(new osgViewer::WindowSizeHandler());

  wm->resizeAllWindows();

  root->addChild(uiCamera);
}

int main(int argc, char* argv[]) {
  osgViewer::Viewer v;
  viewer = &v;
  camera = v.getCamera();

  createScene();
  createUi();

  v.addEventHandler(new osgViewer::StatsHandler);
  v.addEventHandler(new GrabHandler());
  v.setSceneData(root);

  // no camera manipulator
  while (!v.done()) v.frame();
}
