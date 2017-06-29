#include "blender.h"
#include "common.h"
#include "auxiliary.h"
#include "pivot.h"
#include "blenderobject.h"
#include "gridfloor.h"
#include "objectoperation.h"
#include <osg/ValueObject>
#include <osgDB/ReadFile>
#include <osg/CullFace>

namespace zxd {

//------------------------------------------------------------------------------
Blender::Blender()
    : mOutLineColor(osg::Vec4(1.0f, 0.5f, 0.0f, 1.0f)),
      mCamera(0),
      mHudCamera(0),
      mFontSize(12.0f),
      mSelectMask(1 << 0),
      mMiniAxesSize(30.0f) {
  mHudCamera = zxd::createHUDCamera();
  osg::Vec2ui screenSize;
  zxd::getScreenResolution(screenSize[0], screenSize[1]);
  mHudCamera->setProjectionMatrixAsOrtho(
    0, screenSize[0], 0, screenSize[1], -50.0f, 50.0f);
  // compute near far will ignore text
  mHudCamera->setComputeNearFarMode(osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);

  mFont = osgText::readFontFile("fonts/arial.ttf");
  mOpText = zxd::createText(
    mFont, osg::Vec3(10.0f, mFontSize + 10.0f, 0.0f), "", mFontSize);
  mPivotText = zxd::createText(
    mFont, osg::Vec3(500.0f, mFontSize + 10.0f, 0.0f), "", mFontSize);
  mViewText = zxd::createText(mFont,
    osg::Vec3(5.0f, screenSize[1] - 5.0f - mFontSize, 0.0f), "", mFontSize);

  mCursor = new zxd::Cursor();
  addChild(mCursor);

  osg::ref_ptr<osg::Geode> textLeaf = new osg::Geode();
  textLeaf->addDrawable(mOpText);
  textLeaf->addDrawable(mPivotText);
  textLeaf->addDrawable(mViewText);

  mHudCamera->addChild(textLeaf);
  addChild(mHudCamera);

  mGridFloor = new zxd::GridFloor();
  mGridFloor->setScale(5);
  mGridFloor->rebuild();
  mGridFloor->setNodeMask(-1 & ~mSelectMask);

  mPivot = new zxd::Pivot();
  mPivot->setText(mPivotText);
  mPivot->setBlender(this);
  mPivotAxes = new osg::AutoTransform();
  mPivotAxes->setAutoScaleToScreen(true);
  osg::ref_ptr<zxd::Axes> axes = new zxd::Axes();
  axes->setMatrix(osg::Matrix::scale(50.0f, 50.0f, 50.0f));
  mPivotAxes->addChild(axes);
  mPivot->updateText();

  addChild(mGridFloor);
}

//-------------------------------------------------Sekurosufia-----------------------------
void Blender::reputPivotAxes() {
  zxd::removeNodeParents(mPivotAxes);

  // only show axes if there exists selected object
  if (!mCurObject) return;

  switch (mPivot->getType()) {
    case zxd::Pivot::PT_ACTIVEELEMENT:
      mCurObject->addChild(mPivotAxes);
      break;
    case zxd::Pivot::PT_3DCURSOR:
      mCursor->addChild(mPivotAxes);
      break;
    default:
      throw new std::runtime_error("undefined pivot type");
  }
}

//------------------------------------------------------------------------------
void Blender::createMiniAxes() {
  // TODO text become invisible in some angle, don't know why

  // create mini axes
  mMiniAxes = new zxd::Axes;
  mMiniAxes->setLabel(true);
  mMiniAxes->setLineWidth(1.5f);
  // place it at low left corner
  mMiniAxes->setMatrix(
    osg::Matrix::scale(mMiniAxesSize, mMiniAxesSize, mMiniAxesSize) *
    osg::Matrix::translate(mMiniAxesSize * 1.35f, mMiniAxesSize + 1.35f, 0));

  osg::ref_ptr<MiniAxesCallback> cb = new MiniAxesCallback();
  cb->setTargetCamera(mCamera);
  mMiniAxes->setUpdateCallback(cb);

  // osg::StateSet* ss = mMiniAxes->getOrCreateStateSet();
  // osg::ref_ptr<osg::CullFace> cf  = new osg::CullFace();
  // cf->setMode(osg::CullFace::BACK);

  // ss->setMode(GL_CULL_FACE, osg::StateAttribute::OFF);

  mHudCamera->addChild(mMiniAxes);
}

//------------------------------------------------------------------------------
void Blender::setCamera(osg::Camera* v) { mCamera = v; }

//------------------------------------------------------------------------------
void BlendGuiEventhandler::clearRotation() {
  // clear orientation
  osg::Vec3 translation;
  osg::Quat rotation;
  osg::Vec3 scale;
  osg::Quat so;
  getCurObject()->getMatrix().decompose(translation, rotation, scale, so);
  getCurObject()->setMatrix(
    osg::Matrix::scale(scale) * osg::Matrix::translate(translation));
}

//------------------------------------------------------------------------------
void BlendGuiEventhandler::clearTranslation() {
  // clear translation
  const osg::Matrix& m = getCurObject()->getMatrix();
  getCurObject()->postMult(osg::Matrix::translate(-m.getTrans()));
}

//------------------------------------------------------------------------------
void BlendGuiEventhandler::clearScale() {
  osg::Vec3 translation;
  osg::Quat rotation;
  osg::Vec3 scale;
  osg::Quat so;
  getCurObject()->getMatrix().decompose(translation, rotation, scale, so);
  getCurObject()->setMatrix(
    osg::Matrix::rotate(rotation) * osg::Matrix::translate(translation));
}

//------------------------------------------------------------------------------
void BlendGuiEventhandler::selectObject(const osgGA::GUIEventAdapter& ea) {
  // select current object
  osgUtil::IntersectionVisitor iv;
  osg::ref_ptr<osgUtil::LineSegmentIntersector> lsi =
    new osgUtil::LineSegmentIntersector(
      osgUtil::Intersector::WINDOW, ea.getX(), ea.getY());
  iv.setIntersector(lsi);
  iv.setTraversalMask(mBlender->getSelectMask());

  mBlender->getCamera()->accept(iv);

  if (lsi->containsIntersections()) {
    const osgUtil::LineSegmentIntersector::Intersection& result =
      *lsi->getIntersections().begin();

    BlenderObject* curObj = getCurObject();

    for (GLuint i = 0; i < result.nodePath.size(); ++i) {
      osg::Node* node = result.nodePath[i];
      bool isBLenderObject = false;
      if (node->getUserValue("BlenderObject", isBLenderObject)) {
        if (curObj && curObj != node) curObj->deselect();
        curObj = static_cast<zxd::BlenderObject*>(node);
        curObj->select();
        mBlender->setCurObject(curObj);
        break;
      }
    }

  } else {
    // do nothing if nothing hitten
    // getCurObject()->deselect();
  }
}

//------------------------------------------------------------------------------
void BlendGuiEventhandler::placeCursor(const osgGA::GUIEventAdapter& ea) {
  osg::ref_ptr<osgUtil::LineSegmentIntersector> lsi =
    new osgUtil::LineSegmentIntersector(
      osgUtil::Intersector::WINDOW, ea.getX(), ea.getY());
  osgUtil::IntersectionVisitor iv(lsi);
  iv.setTraversalMask(mBlender->getSelectMask());
  mBlender->getCamera()->accept(iv);
  osg::ref_ptr<zxd::Cursor> cursor = mBlender->getCursor();
  osg::Camera* camera = mBlender->getCamera();

  if (lsi->containsIntersections()) {
    const osgUtil::LineSegmentIntersector::Intersection& intersection =
      *lsi->getIntersections().begin();

    // get intersection point in view space
    osg::Vec3 pos = intersection.localIntersectionPoint *
                    osg::computeLocalToWorld(intersection.nodePath);
    cursor->setMatrix(osg::Matrix::translate(pos));
  } else {
    const osg::Matrix& matView = camera->getViewMatrix();
    // get original cursor position in view space
    osg::Vec3 origPos = cursor->getMatrix().getTrans() * matView;

    // get camera ray intersection with near plane
    osg::Matrix matrix;
    if (camera->getViewport())
      matrix.preMult(camera->getViewport()->computeWindowMatrix());
    matrix.preMult(camera->getProjectionMatrix());
    matrix.invert(matrix);

    // point on near plane in view space
    osg::Vec3 p = osg::Vec3(ea.getX(), ea.getY(), 0) * matrix;

    // scale according z to place p at plane that contain origin cursor
    p *= origPos.z() / p.z();

    // now get final world position
    osg::Matrix matInvView = osg::Matrix::inverse(matView);

    cursor->setMatrix(osg::Matrix::translate(p * matInvView));
  }
}

//------------------------------------------------------------------------------
bool BlendGuiEventhandler::handle(
  const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) {
  switch (ea.getEventType()) {
    case osgGA::GUIEventAdapter::KEYDOWN:
      switch (ea.getUnmodifiedKey()) {
        case osgGA::GUIEventAdapter::KEY_G:
          if (!getCurObject()) break;

          if (ea.getModKeyMask() & osgGA::GUIEventAdapter::MODKEY_ALT) {
            clearTranslation();
          } else {
            // do nothing if it's the same operation type
            zxd::ObjectOperation* curOp = getCurOperation();
            if (curOp && curOp->getType() == "grab") break;

            createNewOperation(ea, new zxd::GrabOperation());
          }
          break;
        case osgGA::GUIEventAdapter::KEY_S:
          if (!getCurObject()) break;

          if (ea.getModKeyMask() & osgGA::GUIEventAdapter::MODKEY_ALT) {
            clearScale();
          } else {
            // do nothing if it's the same operation type
            zxd::ObjectOperation* curOp = getCurOperation();
            if (curOp && curOp->getType() == "scale") break;

            createNewOperation(ea, new zxd::ScaleOperation());
          }
          break;
        case osgGA::GUIEventAdapter::KEY_R:
          if (!getCurObject()) break;

          if (ea.getModKeyMask() & osgGA::GUIEventAdapter::MODKEY_ALT) {
            clearRotation();
          } else {
            // do nothing if it's the same operation type
            zxd::ObjectOperation* curOp = getCurOperation();
            if (curOp && curOp->getType() == "rotate") {
              static_cast<zxd::RotateOperation*>(curOp)->toggleMode();
              break;
            }
            createNewOperation(ea, new zxd::RotateOperation());
          }

          break;
        case osgGA::GUIEventAdapter::KEY_Comma:
          // disallow pivot cange during operation
          if (mBlender->getCurOperation()) break;
          mBlender->getPivot()->setType(zxd::Pivot::PT_ACTIVEELEMENT);
          break;
        case osgGA::GUIEventAdapter::KEY_Period:
          if (mBlender->getCurOperation()) break;
          mBlender->getPivot()->setType(zxd::Pivot::PT_3DCURSOR);
          break;

        default:
          break;
      }
      break;
    default:
      break;

    case osgGA::GUIEventAdapter::PUSH:

      if (getCurOperation()) {
        // operation confirm or cancel
        if (ea.getButton() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON) {
          OSG_NOTICE << "confirm operation" << std::endl;
          getCurOperation()->confirm();
        } else {
          getCurOperation()->cancel();
          OSG_NOTICE << "cancel operation" << std::endl;
        }
        mBlender->getMainView()->removeEventHandler(getCurOperation());
        // clear operation
        mBlender->clearOperation();

      } else {
        // select object
        if (ea.getButton() == osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON) {
          selectObject(ea);
        } else if (ea.getButton() ==
                   osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON) {
          // re place cursor
          placeCursor(ea);
        }
      }
  }
  return false;  // return true will stop event
}

//------------------------------------------------------------------------------
void BlendGuiEventhandler::createNewOperation(
  const osgGA::GUIEventAdapter& ea, ObjectOperation* op) {
  OSG_NOTICE << "create new operation " << std::endl;
  ObjectOperation* curOp = getCurOperation();

  if (curOp) {
    // do nothing if curreng operation is the same tyep as T
    if (curOp->getType() == op->getType()) return;

    OSG_NOTICE << "reuse last constrainer " << std::endl;
    // create new operation, use current axis constrainer
    osg::ref_ptr<zxd::AxisConstrainer> ac = curOp->getAxisConstrainer();

    curOp->cancel();
    op->setAxisConstrainer(ac);

  } else {
    op->setAxisConstrainer(new zxd::AxisConstrainer());
  }
  op->setTarget(getCurObject());
  op->setInfoText(mBlender->getOpText());
  op->setBlender(mBlender);

  op->init(osg::Vec2(ea.getX(), ea.getY()));

  zxd::AxisConstrainer* ac = op->getAxisConstrainer();
  ac->setNodeMask(-1 & ~mBlender->getSelectMask());
  mBlender->addChild(ac);
  mBlender->setCurOperation(op);
  OSG_NOTICE << "add constrainer to scene" << std::endl;
  osg::Node* handle = op->getHandle();
  if (handle) {
    handle->setNodeMask(-1 & ~mBlender->getSelectMask());
    mBlender->getHudCamera()->addChild(handle);
  }

  mBlender->getMainView()->addEventHandler(op);
}
}
