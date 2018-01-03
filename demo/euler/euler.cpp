/*
 * Draw world frame with thin line, labled as xyz
 * Draw target frame with thick line, labled as XYZ
 *
 * Reset(R):
 *  1. select type : proper Euler angle or Tait-Bryan angles
 *  2. select rotate sequence:
 *    Euler:   zxz xyz yzy zyz xzx yxy
 *    Tait-Bryan: xyz yzx zxy xzy zyx yxz
 *  3. use trackball to rotate XYZ to desired frame
 *
 *  Play(P):
 *
 *  rotate XYZ to xyz according to rotate type and sequence
 *
 *  issue:
 *    don't understand the detail of how to get euler angle directly
 *    for now, only xyz of Tait-Bryan works.
 *
 */

#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osg/Geometry>
#include <osgDB/ReadFile>
#include <osg/MatrixTransform>
#include <osgGA/GUIEventHandler>
#include "menu.h"
#include "common.h"
#include <osg/LineWidth>
#include <osg/ValueObject>
#include <osgText/Text3D>
#include <osgText/Font3D>
#include <osg/io_utils>
#include <osgAnimation/EaseMotion>
#include "zmath.h"

class AnimCallback;

GLuint rotType = 1;  // 0 for euler, 1 for Tait-Bryan
std::string rotSeq("xyz");
osg::ref_ptr<osg::MatrixTransform> worldFrame;
osg::ref_ptr<osg::MatrixTransform> targetFrame;
osg::ref_ptr<osg::MatrixTransform> animFrame;
osg::ref_ptr<osg::Group> root;
osg::ref_ptr<zxd::Menu> menu;
osg::ref_ptr<osgText::Text> text;
osg::Camera* camera;
osg::ref_ptr<osgGA::CameraManipulator> cameraManipulator;
osg::ref_ptr<osgAnimation::InOutCubicMotion> motion;
osgViewer::Viewer* viewer;
osg::Vec2 p0;
osg::Vec3 xyz;
GLuint xyzIndex = 0;
osg::Vec3 axes[3] = {osg::X_AXIS, osg::Y_AXIS, osg::Z_AXIS};
osg::ref_ptr<AnimCallback> animCallbcak;

bool rotating = false;
bool rotateTargetFrame = false;
bool playing = false;

void updateHelp() {
  std::string help = menu->getCurrentContent();
  help +=
    std::string("\nrotate type : ") + (rotType == 0 ? "Euler" : "Tait-Bryan");
  help += std::string("\nrotate sequence : ") + rotSeq + " \n";
  help += std::string("\nrotating target frame : ") +
          (rotateTargetFrame ? "true" : "false") + " \n";
  help += "\npress r to reset \n";
  help += "press p to play \n";
  help += "press t to rotate target frame \n";
  text->setText(help);
}

class EulerSelectCallback : public zxd::MenuElementSelectCallback {
public:
  EulerSelectCallback() {}
  EulerSelectCallback(const EulerSelectCallback& copy,
    const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY)
      : zxd::MenuElementSelectCallback(copy, copyop) {}
  ~EulerSelectCallback() {}
  META_Object(zxd, EulerSelectCallback);

  virtual void select(zxd::MenuElement* element) {
    std::string s;
    element->getUserValue("data", s);
    std::stringstream ss;
    ss << s[0];
    ss >> rotType;
    rotSeq = s.substr(2);
    updateHelp();
  }
};

osgText::Text3D* createAxisLabel(const osg::Vec3& pos, const std::string s,
  const osg::Vec4& color, GLfloat size) {
  // create label
  static osgText::Font3D* font = osgText::readFont3DFile("fonts/arial.ttf");

  osg::ref_ptr<osgText::Text3D> tx = new osgText::Text3D();
  tx->setFont(font);
  tx->setAxisAlignment(osgText::TextBase::XZ_PLANE);
  tx->setCharacterSize(size);
  tx->setText(s);
  tx->setCharacterDepth(0.01);
  tx->setPosition(pos);
  tx->setColor(color);

  return tx.release();
}

osg::Group* createAxes(
  GLuint lineWidth, GLfloat labelSize = 0, bool capital = false) {
  // create axis
  osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry();
  osg::StateSet* ss = geometry->getOrCreateStateSet();
  ss->setAttributeAndModes(new osg::LineWidth(lineWidth));
  ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

  osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array();
  vertices->push_back(osg::Vec3());
  vertices->push_back(osg::X_AXIS);
  vertices->push_back(osg::Vec3());
  vertices->push_back(osg::Y_AXIS);
  vertices->push_back(osg::Vec3());
  vertices->push_back(osg::Z_AXIS);

  geometry->setVertexArray(vertices);

  osg::ref_ptr<osg::Vec3Array> colors = new osg::Vec3Array();
  colors->push_back(osg::Vec3(1.0f, 0.0f, 0.0f));
  colors->push_back(osg::Vec3(1.0f, 0.0f, 0.0f));
  colors->push_back(osg::Vec3(0.0f, 1.0f, 0.0f));
  colors->push_back(osg::Vec3(0.0f, 1.0f, 0.0f));
  colors->push_back(osg::Vec3(0.0f, 0.0f, 1.0f));
  colors->push_back(osg::Vec3(0.0f, 0.0f, 1.0f));

  geometry->setColorArray(colors);
  geometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

  geometry->addPrimitiveSet(new osg::DrawArrays(GL_LINES, 0, 6));

  osgText::Text3D* x = createAxisLabel(osg::X_AXIS, capital ? "X" : "x",
    osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f), labelSize);
  osgText::Text3D* y = createAxisLabel(osg::Y_AXIS, capital ? "Y" : "y",
    osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f), labelSize);
  osgText::Text3D* z = createAxisLabel(osg::Z_AXIS, capital ? "Z" : "z",
    osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f), labelSize);

  osg::ref_ptr<osg::Group> group = new osg::Group();
  osg::ref_ptr<osg::Geode> gmLeaf = new osg::Geode;
  gmLeaf->addDrawable(geometry);
  group->addChild(gmLeaf);

  osg::ref_ptr<osg::Geode> leafX = new osg::Geode;
  leafX->addDrawable(x);
  osg::ref_ptr<osg::Geode> leafY = new osg::Geode;
  leafY->addDrawable(y);
  osg::ref_ptr<osg::Geode> leafZ = new osg::Geode;
  leafZ->addDrawable(z);

  group->addChild(leafX);
  group->addChild(leafY);
  group->addChild(leafZ);

  return group.release();
}

void reset() {
  animFrame->setNodeMask(0);
  targetFrame->setMatrix(osg::Matrix());
  menu->reset();
  rotType = 0;
  rotSeq = "zxz";
  updateHelp();
}

class AnimCallback : public osg::NodeCallback {
  virtual void operator()(osg::Node* node, osg::NodeVisitor* nv) {
    osg::MatrixTransform* mt = node->asTransform()->asMatrixTransform();
    static GLfloat dt = 0.005f;  // fixed dt

    GLfloat lastValue = motion->getValue();
    motion->update(dt);
    GLfloat dv = motion->getValue() - lastValue;

    mt->preMult(osg::Matrix::rotate(dv * xyz[xyzIndex], axes[xyzIndex]));

    if (motion->getTime() >= 1.0f) {
      motion->reset();
      ++xyzIndex;
      if (xyzIndex == 3) {
        // stop play
        playing = false;
        animFrame->removeUpdateCallback(animCallbcak);
        animFrame->setNodeMask(0);
      }
    }

    traverse(node, nv);
  }
};

void createScene() {
  worldFrame = new osg::MatrixTransform;
  targetFrame = new osg::MatrixTransform;
  animFrame = new osg::MatrixTransform;
  animFrame->setNodeMask(0);  // hide it at beginning

  worldFrame->addChild(createAxes(1, 0.05f, false));
  targetFrame->addChild(createAxes(3, 0.2f, true));
  animFrame->addChild(createAxes(3, 0.2f, true));

  // create menu
  menu = new zxd::Menu();
  menu->setBackKeyString("BackSpace");
  menu->setUniformSelectCallback(new EulerSelectCallback);
  osg::ref_ptr<zxd::SubMenu> rm = menu->getRoot();
  rm->setText("configure rotation");

  zxd::SubMenu* smEuler = rm->addSeqSubMenu("Euler");
  zxd::SubMenu* smTaitBryan = rm->addSeqSubMenu("Tait-Bryan");

  //*    Euler:   zxz xyz yzy zyz xzx yxy
  //*    Tait-Bryan: xyz yzx zxy xzy zyx yxz
  smEuler->addSeqItem("zxz")->setUserValue("data", std::string("0 zxz"));
  smEuler->addSeqItem("xyx")->setUserValue("data", std::string("0 xyx"));
  smEuler->addSeqItem("yzy")->setUserValue("data", std::string("0 yzy"));
  smEuler->addSeqItem("zyz")->setUserValue("data", std::string("0 zyz"));
  smEuler->addSeqItem("xzx")->setUserValue("data", std::string("0 xzx"));
  smEuler->addSeqItem("yxy")->setUserValue("data", std::string("0 yxy"));

  smTaitBryan->addSeqItem("xyz")->setUserValue("data", std::string("1 xyz"));
  smTaitBryan->addSeqItem("yzx")->setUserValue("data", std::string("1 yzx"));
  smTaitBryan->addSeqItem("zxy")->setUserValue("data", std::string("1 zxy"));
  smTaitBryan->addSeqItem("xzy")->setUserValue("data", std::string("1 xzy"));
  smTaitBryan->addSeqItem("zyx")->setUserValue("data", std::string("1 zyx"));
  smTaitBryan->addSeqItem("yxz")->setUserValue("data", std::string("1 yxz"));

  // create help
  osg::Camera* hudCamera = zxd::createHUDCamera();

  GLuint width, height;
  zxd::getScreenResolution(width, height);
  GLuint fontSize = 15;
  GLuint margin = 30;
  osg::Vec3 pos(margin, height - fontSize - margin, 0);

  osg::ref_ptr<osgText::Font> font = osgText::readFontFile("fonts/arial.ttf");
  text = zxd::createText(pos, "", fontSize);
  text->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
  osg::ref_ptr<osg::Geode> leaf = new osg::Geode();
  leaf->addDrawable(text);
  hudCamera->addChild(leaf);

  motion = new osgAnimation::InOutCubicMotion;
  animCallbcak = new AnimCallback;

  updateHelp();

  root = new osg::Group;

  root->addChild(worldFrame);
  root->addChild(targetFrame);
  root->addChild(animFrame);
  root->addChild(hudCamera);
}

void play() {
  if (playing) return;

  xyz = zxd::Math::getEulerXYZ(targetFrame->getMatrix());
  animFrame->setMatrix(osg::Matrix());
  animFrame->setUpdateCallback(animCallbcak);
  animFrame->setNodeMask(-1);
  motion->reset();
  playing = true;
  xyzIndex = 0;
}

class EulerGuiEventHandler : public osgGA::GUIEventHandler {
  virtual bool handle(
    const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) {
    switch (ea.getEventType()) {
      case osgGA::GUIEventAdapter::KEYDOWN:
        switch (ea.getKey()) {
          case osgGA::GUIEventAdapter::KEY_1:
          case osgGA::GUIEventAdapter::KEY_2:
          case osgGA::GUIEventAdapter::KEY_3:
          case osgGA::GUIEventAdapter::KEY_4:
          case osgGA::GUIEventAdapter::KEY_5:
          case osgGA::GUIEventAdapter::KEY_6:
          case osgGA::GUIEventAdapter::KEY_7:
          case osgGA::GUIEventAdapter::KEY_8:
          case osgGA::GUIEventAdapter::KEY_9:
            if (menu->accept(ea.getKey())) updateHelp();
            break;
          case osgGA::GUIEventAdapter::KEY_BackSpace:
            if (menu->back()) updateHelp();
            break;
          case osgGA::GUIEventAdapter::KEY_R:
            reset();
            break;
          case osgGA::GUIEventAdapter::KEY_P:
            play();
            break;
          case osgGA::GUIEventAdapter::KEY_T: {
            rotateTargetFrame = true;
            cameraManipulator = viewer->getCameraManipulator();
            viewer->setCameraManipulator(0);
            updateHelp();
            break;
          }

          default:
            break;
        }
      case osgGA::GUIEventAdapter::PUSH: {
        if (rotateTargetFrame &&
            ea.getButton() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON) {
          rotating = true;
          p0 = osg::Vec2(ea.getXnormalized(), ea.getYnormalized());
          std::cout << p0 << std::endl;
        }
        break;
      }
      case osgGA::GUIEventAdapter::RELEASE: {
        if (rotateTargetFrame &&
            ea.getButton() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON) {
          rotating = false;
          rotateTargetFrame = false;
          updateHelp();
          viewer->setCameraManipulator(cameraManipulator);
        }
        break;
      }
      case osgGA::GUIEventAdapter::DRAG: {
        if (rotateTargetFrame && rotating) {
          osg::Vec2 p1 = osg::Vec2(ea.getXnormalized(), ea.getYnormalized());
          targetFrame->postMult(zxd::Math::arcball(p0, p1, 0.8f));
          p0 = p1;
        }
        break;
      }

      default:
        break;
    }
    return false;  // return true will stop event
  }
};

int main(int argc, char* argv[]) {
  createScene();
  reset();

  osgViewer::Viewer v;
  viewer = &v;
  v.addEventHandler(new EulerGuiEventHandler);
  v.addEventHandler(new osgViewer::StatsHandler);
  v.setSceneData(root);
  camera = v.getCamera();

  return v.run();
}
