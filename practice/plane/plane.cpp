#include <osg/Geode>
#include <osg/Geometry>
#include <common.h>
#include <osgViewer/Viewer>
#include <osg/Texture>
#include <osgDB/ReadFile>
#include <osg/AnimationPath>
#include <osg/MatrixTransform>
#include <osg/io_utils>
#include <osgGA/GUIEventAdapter>
#include <osgText/Font>
#include <osgText/Text>
#include <osg/BlendFunc>
#include <osg/BlendColor>
#include <osgFX/Outline>
#include <osg/Switch>
#include <osgViewer/ViewerEventHandlers>
#include <osgParticle/ParticleSystem>
#include <osgParticle/ModularEmitter>
#include <osgParticle/ParticleSystemUpdater>
#include <osgParticle/AccelOperator>
#include <osg/Point>
#include <osg/PointSprite>
#include <osgShadow/ShadowedScene>
#include <osgShadow/ShadowMap>
#include <osg/PolygonMode>
#include <osgUtil/PrintVisitor>
#include <osg/DisplaySettings>
#include <osg/ShapeDrawable>
#include <osgUtil/IntersectionVisitor>
#include <osgUtil/LineSegmentIntersector>

GLuint rcvShadowMask = 1 << 0;
GLuint castShadowMask = 1 << 1;
GLuint visibilityMask = 1 << 2;
GLuint selectMask = 1 << 3;

osg::ref_ptr<osgShadow::ShadowedScene> scene;

class TruckCallback : public osg::NodeCallback {
  virtual void operator()(osg::Node* node, osg::NodeVisitor* nv) {
    static GLdouble lt = nv->getFrameStamp()->getReferenceTime();
    GLfloat omega = 0.5;
    GLfloat phi = osg::PI;
    GLfloat r = 500;

    osg::MatrixTransform* tf = node->asTransform()->asMatrixTransform();
    GLdouble t = nv->getFrameStamp()->getReferenceTime();
    GLdouble dt = t - lt;
    const osg::Matrix& m = tf->getMatrix();

    osg::Vec3 pos = osg::Vec3(m(3, 0), m(3, 1), m(3, 2)) +
                    osg::Vec3(-r * omega * sinf(t * omega + phi) * dt,
                      r * omega * cosf(t * omega + phi) * dt, 0);
    lt = t;

    tf->setMatrix(osg::Matrix::rotate(t * omega, osg::Z_AXIS) *
                  osg::Matrix::translate(pos));

    traverse(node, nv);
  }
};

class Plane {
private:
  // mPlane<--mTfSpin<--mSwitch<--[mOutline, mGroup]<--mPlane
  //                 <--mParticle
  osg::ref_ptr<osg::Node> mPlane;
  osg::ref_ptr<osg::Switch> mSwitch;
  osg::ref_ptr<osgFX::Outline> mOutline;
  osg::ref_ptr<osg::Group> mGroup;
  osg::ref_ptr<osg::MatrixTransform> mTfSpin;  // spinning control
  osg::ref_ptr<osg::MatrixTransform> mNode;    // outside node
  osg::ref_ptr<osg::Geode> mParticle;
  osg::ref_ptr<osgParticle::ParticleSystem> mPs;

public:
  Plane(bool _createParticle) {
    mPlane = osgDB::readNodeFile("cessna.osg.0,0,180.rot");
    mPlane->setNodeMask(-1 & ~rcvShadowMask);
    mSwitch = new osg::Switch();
    mOutline = new osgFX::Outline();
    mOutline->setWidth(2.0f);
    mOutline->setColor(osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f));
    mGroup = new osg::Group();
    mTfSpin = new osg::MatrixTransform();
    mNode = new osg::MatrixTransform();

    mNode->addChild(mTfSpin);
    mTfSpin->addChild(mSwitch);
    mSwitch->addChild(mOutline, 0);
    mSwitch->addChild(mGroup, 1);
    mOutline->addChild(mPlane);
    mGroup->addChild(mPlane);

    createSpinAnim();

    if (_createParticle) {
      createParticle();
    }
  }

  void toggleOutline() {
    mSwitch->setValue(0, !mSwitch->getValue(0));
    mSwitch->setValue(1, !mSwitch->getValue(1));
  }

  void toggleParticle() {
    if (mParticle) {
      mParticle->setNodeMask(mParticle->getNodeMask() ^ visibilityMask);
    }
  }

  void toggleParticleFrozen() {
    if (mParticle) mPs->setFrozen(!mPs->getFrozen());
  }

  osg::ref_ptr<osg::MatrixTransform> getNode() const { return mNode; }
  void setNode(osg::ref_ptr<osg::MatrixTransform> v) { mNode = v; }

private:
  void createParticle() {
    mParticle = new osg::Geode();

    // create particle
    mPs = new osgParticle::ParticleSystem();
    mPs->setNodeMask(
      visibilityMask | castShadowMask);  // cast shadow not work here
    mPs->getDefaultParticleTemplate().setShape(osgParticle::Particle::POINT);
    mPs->getDefaultParticleTemplate().setLifeTime(5);
    mPs->setParticleScaleReferenceFrame(
      osgParticle::ParticleSystem::LOCAL_COORDINATES);

    // setup particle stateset
    osg::StateSet* ss = mPs->getOrCreateStateSet();

    ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

    osg::ref_ptr<osg::BlendFunc> bf = new osg::BlendFunc();
    bf->setFunction(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    ss->setAttributeAndModes(bf);
    ss->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

    osg::ref_ptr<osg::Texture2D> tex = new osg::Texture2D();
    tex->setImage(osgDB::readImageFile("Images/smoke.rgb"));
    ss->setTextureAttribute(0, tex);

    // create program operator
    osg::ref_ptr<osgParticle::AccelOperator> accel =
      new osgParticle::AccelOperator();
    accel->setToGravity();

    // create program
    osg::ref_ptr<osgParticle::ModularProgram> program =
      new osgParticle::ModularProgram();
    program->setParticleSystem(mPs);
    // program->addOperator(accel);

    // create emtter
    osg::ref_ptr<osgParticle::ModularEmitter> emitter =
      new osgParticle::ModularEmitter();
    emitter->setParticleSystem(mPs);
    emitter->setReferenceFrame(osgParticle::ParticleProcessor::RELATIVE_RF);
    osgParticle::RandomRateCounter* rrc =
      static_cast<osgParticle::RandomRateCounter*>(emitter->getCounter());
    rrc->setRateRange(300, 500);

    osg::ref_ptr<osgParticle::RadialShooter> shooter =
      static_cast<osgParticle::RadialShooter*>(emitter->getShooter());
    GLfloat bias = 0.01f;
    shooter->setThetaRange(osg::PI_2 - bias, osg::PI_2 + bias);
    shooter->setPhiRange(osg::PI_2 * 3 - bias, osg::PI_2 * 3 + bias);
    shooter->setInitialSpeedRange(100.0f, 180.0f);

    // create updater
    osg::ref_ptr<osgParticle::ParticleSystemUpdater> psu =
      new osgParticle::ParticleSystemUpdater();
    psu->addParticleSystem(mPs);

    mParticle = new osg::Geode();
    mParticle->addDrawable(mPs);

    // transform the newly created particles
    mTfSpin->addChild(emitter);
    mTfSpin->addChild(program);
    mTfSpin->addChild(psu);
    // ps will move with it's parent, so in this case, we should add it to
    // scene.
    scene->addChild(mParticle);
  }

  void createSpinAnim() {
    osg::ref_ptr<osg::AnimationPath> animPath = new osg::AnimationPath();

    GLfloat stepTime = 1.0f;
    GLfloat stepAngle = osg::PI_2;

    for (GLuint i = 0; i < 5; ++i) {
      animPath->insert(stepTime * i,
        osg::AnimationPath::ControlPoint(osg::Vec3(0.0f, 0.0f, 0.0f),
                         osg::Quat(stepAngle * i, osg::Y_AXIS)));
    }

    osg::ref_ptr<osg::AnimationPathCallback> apc =
      new osg::AnimationPathCallback();
    apc->setAnimationPath(animPath);
    mTfSpin->setUpdateCallback(apc);
  }
};

typedef std::vector<Plane*> Planes;

class PlaneGroup {
private:
  // mNode<--mTfScane<--mTfCircle
  osg::ref_ptr<osg::MatrixTransform> mTfCircle;  // circle animation
  osg::ref_ptr<osg::MatrixTransform> mTfScale;   // scale
  osg::ref_ptr<osg::Group> mNode;                // toppest parent
  osg::NodeList mParticles;                      // particle nodes

  Planes mPlanes;
  GLuint mNumRow;

public:
  PlaneGroup(GLuint numRows, GLfloat x0, GLfloat y0, GLfloat z0, GLfloat stepX,
    GLfloat stepY)
      : mNumRow(numRows) {
    mNode = new osg::MatrixTransform();
    mTfScale = new osg::MatrixTransform();
    mTfCircle = new osg::MatrixTransform();

    mNode->addChild(mTfScale);
    mTfScale->addChild(mTfCircle);

    {
      // create global alpha. I need glBlendColor to manually control global
      // alpha.
      osg::StateSet* ss = mNode->getOrCreateStateSet();
      ss->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

      osg::ref_ptr<osg::BlendFunc> bf = new osg::BlendFunc();
      bf->setFunction(GL_CONSTANT_COLOR, GL_ONE_MINUS_CONSTANT_COLOR);
      ss->setAttributeAndModes(bf);

      osg::ref_ptr<osg::BlendColor> bc = new osg::BlendColor();
      bc->setConstantColor(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
      ss->setAttributeAndModes(bc);
    }

    // add planes
    for (GLuint row = 0; row < numRows; ++row) {
      GLfloat startX = x0 - row * stepX;
      GLfloat startY = y0 - row * stepY;
      GLuint numPlanes = row * 2 + 1;
      bool lastRow = row == (numRows - 1);
      for (GLuint i = 0; i < numPlanes; ++i) {
        osg::Vec3 pos(startX + i * stepX, startY, z0);
        Plane* plane = new Plane(lastRow);
        mPlanes.push_back(plane);
        osg::MatrixTransform* node = plane->getNode();
        node->setMatrix(osg::Matrix::translate(pos));
        mTfCircle->addChild(node);
        OSG_NOTICE << "add plane at " << pos << std::endl;
      }
    }

    // create circle animation
    osg::ref_ptr<osg::AnimationPathCallback> apc =
      new osg::AnimationPathCallback();
    osg::ref_ptr<osg::AnimationPath> ap = new osg::AnimationPath();
    apc->setAnimationPath(ap);
    ap->setLoopMode(osg::AnimationPath::LOOP);
    GLuint ns = 17;
    GLfloat stepYaw = 2 * osg::PI / (ns - 1);
    GLfloat stepTime = 40.0f / (ns - 1);
    for (GLuint i = 0; i < ns; ++i) {
      GLfloat angle = stepYaw * i;
      osg::Quat q(angle, osg::Z_AXIS);
      ap->insert(stepTime * i,
        osg::AnimationPath::ControlPoint(osg::Vec3(0.0f, 0.0f, 0.0f), q));
    }
    mTfCircle->setUpdateCallback(apc);
  }

  ~PlaneGroup() {
    for (GLuint i = 0; i < mPlanes.size(); ++i) {
      delete mPlanes[i];
    }
    mPlanes.clear();
  }

  void scale(GLfloat f) {
    mTfScale->postMult(osg::Matrix::scale(osg::Vec3(f, f, f)));
  }

  GLfloat getAlpha() {
    osg::StateSet* ss = mNode->getOrCreateStateSet();
    osg::BlendColor* bc = static_cast<osg::BlendColor*>(
      ss->getAttribute(osg::StateAttribute::Type::BLENDCOLOR));
    return bc->getConstantColor()[0];
  }

  void setAlpha(GLfloat f) {
    osg::StateSet* ss = mNode->getOrCreateStateSet();
    osg::BlendColor* bc = static_cast<osg::BlendColor*>(
      ss->getAttribute(osg::StateAttribute::Type::BLENDCOLOR));
    return bc->setConstantColor(osg::Vec4(f, f, f, f));
  }

  void toggleOutline() {
    for (GLuint i = 0; i < mPlanes.size(); ++i) {
      mPlanes[i]->toggleOutline();
    }
  }

  // toggle visible, the particles still exists
  void toggleParticle() {
    for (GLuint i = 0; i < mPlanes.size(); ++i) {
      mPlanes[i]->toggleParticle();
    }
  }

  void toggleParticleFrozen() {
    for (GLuint i = 0; i < mPlanes.size(); ++i) {
      mPlanes[i]->toggleParticleFrozen();
    }
  }

  operator osg::Group*() { return mNode; }
};

#define GROUND_WIDTH 2000

class PlaneGuiEventHandler : public osgGA::GUIEventHandler {
public:
  osg::ref_ptr<osg::Node> mHelp;
  osg::ref_ptr<osg::MatrixTransform> mBox;  // selection box
  PlaneGroup* mPlaneGroup;

public:
  PlaneGuiEventHandler() { createSelectionBox(); }

  osg::ref_ptr<osg::Node> getHelp() const { return mHelp; }
  void setHelp(osg::ref_ptr<osg::Node> v) { mHelp = v; }

  osg::ref_ptr<osg::MatrixTransform> getBox() const { return mBox; }

  PlaneGroup* getPlaneGroup() const { return mPlaneGroup; }
  void setPlaneGroup(PlaneGroup* v) { mPlaneGroup = v; }

  virtual bool handle(
    const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) {
    switch (ea.getEventType()) {
      case osgGA::GUIEventAdapter::KEYDOWN:
        switch (ea.getKey()) {
          case osgGA::GUIEventAdapter::KEY_F1:
            mHelp->setNodeMask(mHelp->getNodeMask() ^ visibilityMask);
            break;
          case osgGA::GUIEventAdapter::KEY_1:
            mPlaneGroup->toggleOutline();
            break;
          case osgGA::GUIEventAdapter::KEY_2:
            mPlaneGroup->toggleParticle();
            break;
          case osgGA::GUIEventAdapter::KEY_3:
            mPlaneGroup->toggleParticleFrozen();
            break;
          case osgGA::GUIEventAdapter::KEY_4: {
            osg::StateSet* ss = scene->getOrCreateStateSet();
            osg::StateAttribute* sa =
              ss->getAttribute(osg::StateAttribute::POLYGONMODE);

            if (!sa) {
              // default mode
              ss->setAttributeAndModes(new osg::PolygonMode(
                osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE));
            } else {
              osg::PolygonMode* pm = static_cast<osg::PolygonMode*>(sa);
              pm->setMode(osg::PolygonMode::FRONT_AND_BACK,
                static_cast<osg::PolygonMode::Mode>(
                            GL_POINT +
                            (pm->getMode(osg::PolygonMode::FRONT_AND_BACK) -
                              GL_POINT + 1) %
                              3));
            }

          } break;
          case osgGA::GUIEventAdapter::KEY_5: {
            osg::ref_ptr<osgUtil::PrintVisitor> pv =
              new osgUtil::PrintVisitor(std::cout);
            scene->accept(*pv);

          } break;
          case osgGA::GUIEventAdapter::KEY_Down:
            mPlaneGroup->scale(0.9f);
            break;
          case osgGA::GUIEventAdapter::KEY_Up:
            mPlaneGroup->scale(1.111111f);
            break;
          case osgGA::GUIEventAdapter::KEY_Left: {
            mPlaneGroup->setAlpha(mPlaneGroup->getAlpha() * 0.9f);
          } break;
          case osgGA::GUIEventAdapter::KEY_Right: {
            mPlaneGroup->setAlpha(mPlaneGroup->getAlpha() * 1.11111f);
          } break;
          default:
            break;
        }
        break;

      case osgGA::GUIEventAdapter::RELEASE:
        if (ea.getButton() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON &&
            ea.getModKeyMask() & osgGA::GUIEventAdapter::MODKEY_LEFT_CTRL) {
          osgUtil::IntersectionVisitor iv;
          iv.setTraversalMask(selectMask);

          osg::ref_ptr<osgUtil::LineSegmentIntersector> lsi =
            new osgUtil::LineSegmentIntersector(
              osgUtil::Intersector::WINDOW, ea.getX(), ea.getY());
          iv.setIntersector(lsi);

          osg::Camera* camera =
            static_cast<osgViewer::Viewer*>(&aa)->getCamera();
          camera->accept(iv);

          if (mBox->getNumParents() != 0) 
            mBox->getParent(0)->removeChild(mBox);

          if (lsi->getIntersections().empty()) {
            mBox->setNodeMask(0);  // just hide the box
          } else {
            // scale and show box
            const osgUtil::LineSegmentIntersector::Intersection& is =
              *(lsi->getIntersections().begin());
            osg::Drawable* drawable = is.drawable;
            const osg::BoundingBox& bb = drawable->getBoundingBox();
            //assume last 2nd node is group or it's descendent
            osg::Group* group = (*(is.nodePath.rbegin() + 1))->asGroup();
            if (!group) break;

            osg::Matrix matrix =
              osg::Matrix::scale(osg::Vec3(bb.xMax() - bb.xMin(),
                bb.yMax() - bb.yMin(), bb.zMax() - bb.zMin())) *
              osg::Matrix::translate(bb.center());

            mBox->setMatrix(matrix);
            mBox->setNodeMask(visibilityMask);
            group->addChild(mBox);
          }
        }
        break;

      default:
        break;
    }

    return false;  // return true will stop event
  }

  void createSelectionBox() {
    if (mBox) return;

    osg::ref_ptr<osg::ShapeDrawable> sd = new osg::ShapeDrawable();
    sd->setShape(new osg::Box(osg::Vec3(), 1.0f));

    osg::StateSet* ss = sd->getOrCreateStateSet();
    ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

    ss->setAttributeAndModes(new osg::PolygonMode(
      osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE));

    osg::ref_ptr<osg::Geode> leaf = new osg::Geode();
    leaf->addDrawable(sd);

    mBox = new osg::MatrixTransform();
    mBox->addChild(leaf);
    mBox->setNodeMask(0);
  }
};

osg::Node* createHelp() {
  GLuint sw = 800, sh = 600;
  zxd::getScreenResolution(sw, sh);

  osg::ref_ptr<osgText::Font> font = osgText::readFontFile("fonts/arial.ttf");
  const std::string helpContent =
    " f1 to toggle help \n"
    " 1 to toggle outline. \n"
    " 2 to toggle particle visible \n"
    " 3 to toggle particle frozen \n"
    " 4 to change polygon mode \n"
    " 5 to print \n"
    " ctrl+lmb to select item \n"
    " left/right to change alpha  \n"
    " up/down to change scale \n";

  GLfloat fontSize = 15.0f;  // font height
  GLfloat margin = 10.0f;
  osgText::Text* text = zxd::createText(font,
    osg::Vec3(margin, sh - fontSize - margin, 0.0f), helpContent, fontSize);
  osg::ref_ptr<osg::Geode> help = new osg::Geode();
  help->addDrawable(text);
  help->setNodeMask(visibilityMask);
  return help.release();
}

osg::Node* createGround() {
  GLfloat hw = 0.5f * GROUND_WIDTH;
  GLfloat hh = hw;

  osg::ref_ptr<osg::Image> imgMoon = osgDB::readImageFile("Images/lz.rgb");
  osg::ref_ptr<osg::Texture2D> texGround = new osg::Texture2D();
  texGround->setImage(imgMoon);

  osg::Geometry* gmGround =
    osg::createTexturedQuadGeometry(osg::Vec3(-hw, -hh, 0.0f),
      osg::X_AXIS * GROUND_WIDTH, osg::Y_AXIS * GROUND_WIDTH);

  osg::StateSet* ss = gmGround->getOrCreateStateSet();
  ss->setTextureAttributeAndModes(0, texGround);

  osg::ref_ptr<osg::Geode> ground = new osg::Geode();
  ground->addDrawable(gmGround);
  //if you set node mask at drawalbe, it will be selected.
  ground->setNodeMask(visibilityMask | rcvShadowMask);
  return ground.release();
}

osg::Node* createTruck() {
  osg::ref_ptr<osg::MatrixTransform> truck = new osg::MatrixTransform();
  osg::ref_ptr<osg::MatrixTransform> truckScale = new osg::MatrixTransform();
  truckScale->setMatrix(osg::Matrix::scale(osg::Vec3(5.0f, 5.0f, 5.0f)));
  truckScale->addChild(osgDB::readNodeFile("dumptruck.osg.0,0,-90.rot"));
  truck->addChild(truckScale);
  truck->setMatrix(osg::Matrix::translate(-500, 0, 50));
  truck->setUpdateCallback(new TruckCallback());
  truck->setNodeMask(-1);
  return truck.release();
}

int main(int argc, char* argv[]) {
  /*
   * create help
   */
  scene = new osgShadow::ShadowedScene();
  osg::ref_ptr<osg::Node> help = createHelp();

  // get screen resolution
  GLuint sw = 800, sh = 600;
  zxd::getScreenResolution(sw, sh);
  osg::Camera* camera = zxd::createHUDCamera(0, sw, 0.0f, sh);
  camera->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
  camera->addChild(help);
  camera->setNodeMask(visibilityMask);

  /*
   * create a simple ground
   */

  PlaneGroup pg(4, 500, 0, 200, 50, 50);

  // shadow
  osg::ref_ptr<osg::LightSource> ls = new osg::LightSource();
  ls->getLight()->setPosition(osg::Vec4(0.0f, 0.0f, 200.0f, 0.0f));
  ls->getLight()->setAmbient(osg::Vec4(0.2f, 0.2f, 0.2f, 1.0f));
  ls->getLight()->setDiffuse(osg::Vec4(0.8f, 0.8f, 0.8f, 1.0f));

  osg::ref_ptr<osgShadow::ShadowMap> shadowMap = new osgShadow::ShadowMap();
  shadowMap->setLight(ls);
  shadowMap->setTextureSize(osg::Vec2s(1024, 1024));
  shadowMap->setTextureUnit(1);

  scene->setShadowTechnique(shadowMap);
  // becareful here shadow mask must | with visibility mask!!!!!!
  scene->setReceivesShadowTraversalMask(rcvShadowMask | visibilityMask);
  scene->setCastsShadowTraversalMask(castShadowMask | visibilityMask);

  scene->addChild(ls);
  scene->addChild(createGround());
  scene->addChild(pg);
  scene->addChild(createTruck());

  osg::ref_ptr<PlaneGuiEventHandler> handler = new PlaneGuiEventHandler();
  handler->setHelp(help);
  handler->setPlaneGroup(&pg);

  osgViewer::Viewer viewer;
  viewer.addEventHandler(handler);
  // enable stencil buffer
  osg::DisplaySettings::instance()->setMinimumNumStencilBits(1);
  viewer.getCamera()->setClearMask(
    GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  viewer.getCamera()->setCullMask(visibilityMask);

  // fps
  viewer.addEventHandler(new osgViewer::StatsHandler());

  osg::DisplaySettings::instance()->setNumMultiSamples(4);

  // don't add text to shadowed scene, it will make it disappeared
  osg::ref_ptr<osg::Group> root = new osg::Group();
  root->addChild(scene);
  root->addChild(camera);

  viewer.setSceneData(root);
  return viewer.run();
}
