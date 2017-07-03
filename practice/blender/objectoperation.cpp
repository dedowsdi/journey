#include "objectoperation.h"
#include "blender.h"
#include "pivot.h"
#include "zmath.h"
#include "prerequisite.h"

namespace zxd {

//------------------------------------------------------------------------------
void ObjectOperation::init(osgViewer::View* view, osg::Vec2 v) {
  setBlenderView(view);
  setCamera(mBlenderView->getCamera());
  mBlenderView->addEventHandler(this);

  mOrtho = zxd::Math::isOrthoProj(mCamera->getProjectionMatrix());

  setPivot(sgBlender->getPivot());
  setTarget(sgBlender->getCurObject());
  setInfoText(sgBlender->getInfoView()->getOpText());

  mModel = mTarget->getMatrix();
  mInvModel.invert(mModel);

  mModelRS = zxd::Math::getMatRS(mModel);
  mInvModelRS = zxd::Math::getMatRS(mInvModel);
  mModelT = zxd::Math::getMatT(mModel);

  mView = mCamera->getViewMatrix();
  mInvView.invert(mView);

  mViewRS = zxd::Math::getMatRS(mView);
  mInvViewRS = zxd::Math::getMatRS(mInvView);

  mCameraPos = mInvView.getTrans();

  mWnd = mCamera->getViewport()->computeWindowMatrix();
  mInvWnd.invert(mWnd);

  mProj = mCamera->getProjectionMatrix();
  mInvProj.invert(mProj);

  mProjWnd = mProj * mWnd;
  mInvProjWnd.invert(mProjWnd);

  mViewProjWnd = mView * mProjWnd;
  mInvViewProjWnd.invert(mViewProjWnd);

  mModelView = mModel * mView;
  mInvModelView.invert(mModelView);
  mModelCameraPos = mInvModelView.getTrans();

  mModelViewProjWnd = mModelView * mProjWnd;
  mInvModelViewProjWnd.invert(mModelViewProjWnd);

  mWorldStartObject = mModel.getTrans();

  osg::Vec3 p = mWorldStartObject * mViewProjWnd;
  mWndStartObject.x() = p.x();
  mWndStartObject.y() = p.y();

  mWorldPivot = mPivot->getPivot();
  mLocalPivot = mWorldPivot * mInvModel;
  osg::Vec3 wndPivot = mWorldPivot * mViewProjWnd;
  mWndPivot[0] = wndPivot[0];
  mWndPivot[1] = wndPivot[1];

  mWorldToPivot = osg::Matrix::translate(-mWorldPivot);
  mInvWorldToPivot = osg::Matrix::translate(mWorldPivot);
  mLocalToPivot = osg::Matrix::translate(-mLocalPivot);
  mInvLocalToPivot = osg::Matrix::translate(mLocalPivot);

  OSG_NOTICE << "mWorldPivot" << mWorldPivot << std::endl;
  OSG_NOTICE << "mLocalPivot" << mLocalPivot << std::endl;

  GLfloat left, right, bottom, top, far;
  mProj.getFrustum(left, right, bottom, top, mNear, far);
  OSG_NOTICE << "left:" << left << " right :" << right << std::endl;

  mViewStartObject = mModel.getTrans() * mView;
  mNearPlaneStartObject =
    mViewStartObject * (-mNear / mViewStartObject.z());  // scale to near

  mCurrentCursor = mStartCursor = mLastCursor = v;

  mNearPlaneStartCursor =
    osg::Vec3(mStartCursor.x(), mStartCursor.y(), 0) * mInvProjWnd;

  // init axis constrainer position
  mAc->setMatrix(osg::Matrix::translate(mWorldPivot));

  doInit();

  applyAxisConstrainerChange();
}

//------------------------------------------------------------------------------
void ObjectOperation::setAxisConstrainer(AxisConstrainer* v) { mAc = v; }

//------------------------------------------------------------------------------
void ObjectOperation::cancel() {
  mTarget->setMatrix(mModel);
  clean();
}

//------------------------------------------------------------------------------
void ObjectOperation::update(bool shiftDown) {
  doUpdate(shiftDown);
  updateText();
  updateHandle();
}

//------------------------------------------------------------------------------
void ObjectOperation::setTarget(osg::ref_ptr<zxd::BlenderObject> v) {
  mTarget = v;
}

//------------------------------------------------------------------------------
void ObjectOperation::setPivot(Pivot* v) { mPivot = v; }

//------------------------------------------------------------------------------
bool ObjectOperation::handle(
  const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) {
  bool shiftDown = ea.getModKeyMask() & osgGA::GUIEventAdapter::MODKEY_SHIFT;
  switch (ea.getEventType()) {
    case osgGA::GUIEventAdapter::KEYDOWN:
      switch (ea.getUnmodifiedKey()) {
        case osgGA::GUIEventAdapter::KEY_X:
          mAc->addConstrain(
            shiftDown ? AxisConstrainer::CT_YZ : AxisConstrainer::CT_X);
          applyAxisConstrainerChange();
          updateText();
          break;
        case osgGA::GUIEventAdapter::KEY_Y:
          mAc->addConstrain(
            shiftDown ? AxisConstrainer::CT_ZX : AxisConstrainer::CT_Y);
          applyAxisConstrainerChange();
          updateText();
          break;
        case osgGA::GUIEventAdapter::KEY_Z:
          mAc->addConstrain(
            shiftDown ? AxisConstrainer::CT_XY : AxisConstrainer::CT_Z);
          applyAxisConstrainerChange();
          updateText();
          break;
        case osgGA::GUIEventAdapter::KEY_Shift_L:
        case osgGA::GUIEventAdapter::KEY_Shift_R:
          onShiftDown();
          break;
        default:
          break;
      }
      break;
    case osgGA::GUIEventAdapter::MOVE: {
      mEa1 = &ea;
      setCurrentCursor(osg::Vec2(ea.getX(), ea.getY()));
      update(ea.getModKeyMask() & osgGA::GUIEventAdapter::MODKEY_LEFT_SHIFT);
      mLastCursor = mCurrentCursor;
      mEa0 = mEa0;
    } break;

    default:
      break;
  }
  return false;  // return true will stop event
}

//------------------------------------------------------------------------------
void ObjectOperation::retransfromAxisConstrainer() {
  // rotate and translate to pivot point
  if (mAc->getFrame() == zxd::AxisConstrainer::CF_LOCAL)
    mAc->setMatrix(
      zxd::Math::getMatR(mModel) * osg::Matrix::translate(mWorldPivot));
  else
    mAc->setMatrix(osg::Matrix::translate(mWorldPivot));
}

//------------------------------------------------------------------------------
void ObjectOperation::clean() {
  // remove axis constrainer and handle from scene
  zxd::removeNodeParents(mAc);
  OSG_NOTICE << "clear axis constrainer" << std::endl;

  osg::Node* handle = getHandle();
  if (handle) {
    zxd::removeNodeParents(handle);
    OSG_NOTICE << "clear handle" << std::endl;
  }
  mBlenderView->removeEventHandler(this);
}

//------------------------------------------------------------------------------
osg::Plane GrabOperation::getConstrainPlane() {
  osg::Vec4 pv;

  osg::Vec3 v;
  switch (mAc->getType()) {
    case zxd::AxisConstrainer::CT_XY:
      v = osg::Z_AXIS;
      break;
    case zxd::AxisConstrainer::CT_YZ:
      v = osg::X_AXIS;
      break;
    case zxd::AxisConstrainer::CT_ZX:
      v = osg::Y_AXIS;
      break;
    default:
      throw std::runtime_error("unknown constrain type");
  }

  if (mAc->getFrame() == zxd::AxisConstrainer::CF_LOCAL) {
    return osg::Plane(v, 0);
  } else {
    return osg::Plane(v, mWorldStartObject);
  }
}

//------------------------------------------------------------------------------
void GrabOperation::getConstrainPlaneAxes(osg::Vec3& v0, osg::Vec3& v1) {
  switch (mAc->getType()) {
    case zxd::AxisConstrainer::CT_XY:
      v0 = osg::X_AXIS;
      v1 = osg::Y_AXIS;
      break;
    case zxd::AxisConstrainer::CT_YZ:
      v0 = osg::Y_AXIS;
      v1 = osg::Z_AXIS;
      break;
    case zxd::AxisConstrainer::CT_ZX:
      v0 = osg::Z_AXIS;
      v1 = osg::X_AXIS;
      break;
    default:
      throw std::runtime_error("unknown constrain type");
  }
}

//------------------------------------------------------------------------------
osg::Vec3 GrabOperation::getConstrainAxis() {
  osg::Vec3 v;
  switch (mAc->getType()) {
    case zxd::AxisConstrainer::CT_X:
      v = osg::X_AXIS;
      break;
    case zxd::AxisConstrainer::CT_Y:
      v = osg::Y_AXIS;
      break;
    case zxd::AxisConstrainer::CT_Z:
      v = osg::Z_AXIS;
      break;
    default:
      throw std::runtime_error("unknown constrain type");
  }
  return v;
}

//------------------------------------------------------------------------------
void GrabOperation::computeOffset(bool shiftDown) {
  osg::Vec3 endPos;  // endPos in local or world
  switch (mAc->getType()) {
    case zxd::AxisConstrainer::CT_NONE: {
      // get near plane final projection place, than scale to get view
      // position
      osg::Vec2 wndEndPos = getWndEndPos();
      osg::Vec3 npEndPos =
        osg::Vec3(wndEndPos.x(), wndEndPos.y(), 0) * mInvProjWnd;

      osg::Vec3 viewEndPos = npEndPos;

      if (getOrtho())
        viewEndPos.z() = mViewStartObject.z();
      else
        viewEndPos *= (mViewStartObject.z() / -mNear);

      endPos = viewEndPos * mInvView;
    } break;

    case zxd::AxisConstrainer::CT_X:
    case zxd::AxisConstrainer::CT_Y:
    case zxd::AxisConstrainer::CT_Z: {
      // get camera ray in world or local frame, skew with constrained axis
      osg::Vec3 rayStart, dir;
      getCameraRay(rayStart, dir);
      // get target world and wnd axis
      osg::Vec3 constrainAxis = getConstrainAxis();

      endPos = cameraRayTestSingleAxis(rayStart, dir, constrainAxis);
    } break;

    case zxd::AxisConstrainer::CT_YZ:
    case zxd::AxisConstrainer::CT_ZX:
    case zxd::AxisConstrainer::CT_XY: {
      // get camra ray in world or local frame, interset with constrained
      osg::Vec3 rayStart, dir;
      getCameraRay(rayStart, dir);

      osg::Plane constrainPlane = getConstrainPlane();
      // find intersection between ray and constrain plane
      zxd::RayPlaneRel res =
        zxd::Math::getLinePlaneRelation(rayStart, dir, constrainPlane.asVec4());

      if (res.type && res.t > 0.0001f)
        endPos = res.ip;
      else {
        // no intersection found, this happens when constrain plane parallel to
        // camera ray. Use the axis that has bigger angle with camera ray ,and
        // find
        // the skew point. eg: X, top ortho view
        osg::Vec3 axis0, axis1, *axis;
        getConstrainPlaneAxes(axis0, axis1);
        axis =
          zxd::Math::angleLine(axis0, dir) > zxd::Math::angleLine(axis1, dir)
            ? &axis0
            : &axis1;
        endPos = cameraRayTestSingleAxis(rayStart, dir, *axis);
      }

    } break;
    default:
      throw std::runtime_error("unknown constrain type");
  }

  mOffset = endPos - getObjectStart();
  if (shiftDown) {
    mOffset = mOffsetShift + (mOffset - mOffsetShift) * 0.1f;
  }
}

//------------------------------------------------------------------------------
osg::Vec3 GrabOperation::cameraRayTestSingleAxis(
  const osg::Vec3& rayStart, const osg::Vec3& dir, const osg::Vec3& axis) {
  osg::Vec3 objStart = getObjectStart();

  zxd::RayRel res =
    zxd::Math::getLineRelation(rayStart, dir, objStart, axis);

  if (res.type && res.t0 > 0.0001f)
    return res.sk1;
  else {
    // endPos = mTarget->getMatrix().getTrans();

    // if camera ray and constrain axis parallel to each other or they intersect
    // at near rayStart(only happen for persp camPos for persp) simply use
    // vertical offset to decide translation. eg: z top view in both persp
    // and ortho

    float dy = mCurrentCursor[1] - mStartCursor[1];
    return objStart + axis * dy;
  }
}

//------------------------------------------------------------------------------
void GrabOperation::doUpdate(bool shiftDown) {
  mInvCurModel.invert(mTarget->getMatrix());
  computeOffset(shiftDown);
  translateObject();
}

//------------------------------------------------------------------------------
osg::Vec3 GrabOperation::getAccumulatedOffset() {
  // get current translation
  const osg::Matrix& m = mTarget->getMatrix();
  osg::Vec3 currentTranslation =
    mAc->getFrame() == zxd::AxisConstrainer::CF_LOCAL
      ? osg::Matrix::transform3x3(m.getTrans(), mInvModel)
      : m.getTrans();

  // get difference between current and initial translation
  osg::Vec3 startTranslation =
    mAc->getFrame() == zxd::AxisConstrainer::CF_LOCAL
      ? osg::Matrix::transform3x3(mModel.getTrans(), mInvModel)
      : mModel.getTrans();

  return currentTranslation - startTranslation;
}

//------------------------------------------------------------------------------
void GrabOperation::updateText() {
  std::string frame = zxd::toString(mAc->getFrame());
  std::stringstream ss;
  ss.precision(4);
  ss.setf(std::ios_base::fixed);

  // osg::Vec3 offset = getAccumulatedOffset();
  osg::Vec3 offset = mOffset;

  float l = offset.length();

  static const char* axes[] = {"X", "Y", "Z"};

  if (mInfoText) {
    switch (mAc->getType()) {
      case zxd::AxisConstrainer::CT_NONE:
        ss << "Dx:" << offset.x() << " Dy:" << offset.y()
           << " Dz:" << offset.z() << " (" << l << ")";
        break;
      case zxd::AxisConstrainer::CT_X:
      case zxd::AxisConstrainer::CT_Y:
      case zxd::AxisConstrainer::CT_Z: {
        int idx = mAc->getType() - zxd::AxisConstrainer::CT_X;
        ss << "D:" << offset[idx] << " (" << l << ") along " << frame << " "
           << axes[idx];
      } break;
      case zxd::AxisConstrainer::CT_YZ:
      case zxd::AxisConstrainer::CT_ZX:
      case zxd::AxisConstrainer::CT_XY: {
        int idx = mAc->getType() - zxd::AxisConstrainer::CT_X;
        ss << "D:" << offset[(idx + 1) % 3] << " D:" << offset[(idx + 2) % 3]
           << " (" << l << ") locking " << frame << " " << axes[idx];
      } break;
    }

    mInfoText->setText(ss.str());
  }
}

//------------------------------------------------------------------------------
void GrabOperation::doApplyAxisConstrainerChange() {
  // proj offset to constrained axes
  osg::ref_ptr<osg::Vec3Array> axes = new osg::Vec3Array;
  switch (mAc->getType()) {
    case zxd::AxisConstrainer::CT_NONE:
      // do nothing
      return;
    case zxd::AxisConstrainer::CT_X:
      axes->push_back(osg::X_AXIS);
      break;
    case zxd::AxisConstrainer::CT_Y:
      axes->push_back(osg::Y_AXIS);
      break;
    case zxd::AxisConstrainer::CT_Z:
      axes->push_back(osg::Z_AXIS);
      break;
    case zxd::AxisConstrainer::CT_YZ:
      axes->push_back(osg::Y_AXIS);
      axes->push_back(osg::Z_AXIS);
      break;
    case zxd::AxisConstrainer::CT_ZX:
      axes->push_back(osg::Z_AXIS);
      axes->push_back(osg::X_AXIS);
      break;
    case zxd::AxisConstrainer::CT_XY:
      axes->push_back(osg::X_AXIS);
      axes->push_back(osg::Y_AXIS);
      break;
  }

  GLfloat offsetShiftRatio = 0.0f;
  if (mOffsetShift != osg::Vec3() && mOffset != osg::Vec3()) {
    offsetShiftRatio = mOffsetShift.length() / mOffset.length();
  }

  osg::Vec3 offset = getAccumulatedOffset();

  // reset to start position
  mTarget->setMatrix(mModel);

  mOffset = osg::Vec3();
  std::for_each(axes->begin(), axes->end(), [&](decltype(*axes->begin()) axis) {
    mOffset = mOffset + axis * (offset * axis);
  });

  // get new offsetShift
  if (offsetShiftRatio != 0.0f) mOffsetShift = mOffset * offsetShiftRatio;

  translateObject();
}

//------------------------------------------------------------------------------
void GrabOperation::translateObject() {
  /*
   * pivot contains only translation, it won't affect translation behavior
   */
  if (mAc->getFrame() == zxd::AxisConstrainer::CF_LOCAL)
    mTarget->setMatrix(osg::Matrix::translate(mOffset) * mModel);
  else
    mTarget->setMatrix(mModel * osg::Matrix::translate(mOffset));
}

//------------------------------------------------------------------------------
void ScaleOperation::doInit() {
  mInitalHandleLength = (mCurrentCursor - mWndStartObject).length();
  OSG_NOTICE << "initial handle length " << mInitalHandleLength << std::endl;
  mLastHandleLength = mInitalHandleLength;

  // create handle
  mHandle = new osg::Group();

  // handle line
  osg::ref_ptr<zxd::LineSegmentNode> lsn = new zxd::LineSegmentNode();
  osg::Vec3 v0, v1;
  getHandlePosition(v0, v1);
  lsn->setStartPosition(v0);

  osg::StateSet* ss = lsn->getOrCreateStateSet();
  ss->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
  ss->setAttributeAndModes(new osg::LineStipple(1.0f, 0xF0F0));

  // position direction arrow at cursor place
  osg::ref_ptr<zxd::DirectionArrow> da = new zxd::DirectionArrow();

  mHandle->addChild(lsn);
  mHandle->addChild(da);
  updateHandle();

  OSG_NOTICE << "create scale handle" << std::endl;
}

//------------------------------------------------------------------------------
void ScaleOperation::updateHandle() {
  // change handle and arrow direction
  zxd::LineSegmentNode* lsn =
    static_cast<zxd::LineSegmentNode*>(mHandle->getChild(0));
  zxd::DirectionArrow* da =
    static_cast<zxd::DirectionArrow*>(mHandle->getChild(1));

  osg::Vec3 v0, v1;
  getHandlePosition(v0, v1);

  lsn->setEndPosition(v1);
  // OSG_NOTICE << "set handle end position : " << v1 << std::endl;

  da->setMatrix(osg::Matrix::translate(v1));
  da->setDirection(0, mCurrentCursor - mWndPivot);
  da->setDirection(1, mWndPivot - mCurrentCursor);
}

//------------------------------------------------------------------------------
void ScaleOperation::doUpdate(bool shiftDown) {
  // get scale change
  GLfloat curHandleLength = (mCurrentCursor - mWndStartObject).length();
  GLfloat dtScale = (curHandleLength - mLastHandleLength) / mInitalHandleLength;
  mLastHandleLength = curHandleLength;

  mScale += shiftDown ? dtScale * 0.1f : dtScale;

  scaleObject();
}

//------------------------------------------------------------------------------
osg::Vec3 ScaleOperation::getScaleVector() {
  osg::Vec3 scaleVector;
  switch (mAc->getType()) {
    case AxisConstrainer::CT_NONE:
      scaleVector = osg::Vec3(mScale, mScale, mScale);
      break;
    case AxisConstrainer::CT_X:
      scaleVector = osg::Vec3(mScale, 1, 1);
      break;
    case AxisConstrainer::CT_Y:
      scaleVector = osg::Vec3(1, mScale, 1);
      break;
    case AxisConstrainer::CT_Z:
      scaleVector = osg::Vec3(1, 1, mScale);
      break;
    case AxisConstrainer::CT_YZ:
      scaleVector = osg::Vec3(1, mScale, mScale);
      break;
    case AxisConstrainer::CT_ZX:
      scaleVector = osg::Vec3(mScale, 1, mScale);
      break;
    case AxisConstrainer::CT_XY:
      scaleVector = osg::Vec3(mScale, mScale, 1);
      break;
    default:
      throw std::runtime_error("unknown constrain type");
  }
  return scaleVector;
}

//------------------------------------------------------------------------------
void ScaleOperation::scaleObject() {
  osg::Vec3 scaleVector = getScaleVector();
  osg::Matrix scaleMatrix = osg::Matrix::scale(scaleVector);

  if (mAc->getFrame() == AxisConstrainer::CF_LOCAL)
    mTarget->setMatrix(mLocalToPivot * scaleMatrix * mInvLocalToPivot * mModel);
  else
    mTarget->setMatrix(mModel * mWorldToPivot * scaleMatrix * mInvWorldToPivot);
}

//------------------------------------------------------------------------------
void ScaleOperation::updateText() {
  std::string frame = zxd::toString(mAc->getFrame());
  std::stringstream ss;
  ss << "Scale ";
  ss.precision(4);
  ss.setf(std::ios_base::fixed);

  static const char* axes[] = {"X", "Y", "Z"};

  if (mInfoText) {
    switch (mAc->getType()) {
      case zxd::AxisConstrainer::CT_NONE:
        ss << "X:" << mScale << " Y:" << mScale << " Z:" << mScale;
        break;
      case zxd::AxisConstrainer::CT_X:
      case zxd::AxisConstrainer::CT_Y:
      case zxd::AxisConstrainer::CT_Z:
        ss << mScale << " along " << frame << " "
           << axes[mAc->getType() - zxd::AxisConstrainer::CT_X];
        break;
      case zxd::AxisConstrainer::CT_YZ:
      case zxd::AxisConstrainer::CT_ZX:
      case zxd::AxisConstrainer::CT_XY:
        ss << mScale << " : " << mScale << " locking " << frame << " "
           << axes[mAc->getType() - zxd::AxisConstrainer::CT_YZ];
        break;
    }

    mInfoText->setText(ss.str());
  }
}

//------------------------------------------------------------------------------
void RotateOperation::setMode(int v) {
  mMode = v;
  // clear transform during switch
  mTarget->setMatrix(mModel);
  mRot = 0;
  if (mMode == 0)
    applyAxisConstrainerChange();
  else
    mTrackballCursor = mCurrentCursor;

  mHandleSwitch->setSingleChildOn(mMode);
  updateHandle();
}

//------------------------------------------------------------------------------
void RotateOperation::updateHandleTrackball() {
  osg::Group* handle = mHandleSwitch->getChild(1)->asGroup();

  zxd::DirectionArrow* da0 =
    static_cast<zxd::DirectionArrow*>(handle->getChild(0));
  zxd::DirectionArrow* da1 =
    static_cast<zxd::DirectionArrow*>(handle->getChild(1));
  osg::Vec3 v0, v1;
  getHandlePosition(v0, v1);
  da0->setMatrix(osg::Matrix::translate(v1));
  da1->setMatrix(osg::Matrix::translate(v1));
}

//------------------------------------------------------------------------------
void RotateOperation::updateHandleXYZ() {
  osg::Group* handle = mHandleSwitch->getChild(0)->asGroup();

  zxd::LineSegmentNode* lsn =
    static_cast<zxd::LineSegmentNode*>(handle->getChild(0));
  zxd::DirectionArrow* da =
    static_cast<zxd::DirectionArrow*>(handle->getChild(1));

  // update handle line
  osg::Vec3 v0, v1;
  getHandlePosition(v0, v1);
  lsn->setEndPosition(v1);

  // upate handle arrow direction, which perps to handle line
  osg::Vec2 r = mCurrentCursor - mWndPivot;
  osg::Vec2 dir(-r[1], r[0]);
  da->setDirection(0, dir);
  da->setDirection(1, -dir);
  da->setMatrix(osg::Matrix::translate(v1));
}

//------------------------------------------------------------------------------
void RotateOperation::doInit() {
  // create handle
  mHandle = new osg::Group;
  mHandleSwitch = new osg::Switch;

  osg::Vec3 v0, v1;
  getHandlePosition(v0, v1);

  // create xyz handle
  {
    osg::ref_ptr<osg::Group> xyzHandle = new osg::Group();
    // handle line
    osg::ref_ptr<zxd::LineSegmentNode> lsn = new zxd::LineSegmentNode();
    lsn->setPosition(v0, v1);

    osg::StateSet* ss = lsn->getOrCreateStateSet();
    ss->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
    ss->setAttributeAndModes(new osg::LineStipple(1.0f, 0xF0F0));

    // position direction arrow at cursor place
    osg::ref_ptr<zxd::DirectionArrow> da = new zxd::DirectionArrow();
    da->setMatrix(osg::Matrix::translate(v1));

    xyzHandle->addChild(lsn);
    xyzHandle->addChild(da);
    mHandleSwitch->addChild(xyzHandle, mMode == 0);
  }

  // create trackball handle
  {
    osg::ref_ptr<osg::Group> tbHandle = new osg::Group();
    // handle line

    // position direction arrow at cursor place
    osg::ref_ptr<zxd::DirectionArrow> da0 = new zxd::DirectionArrow();
    osg::ref_ptr<zxd::DirectionArrow> da1 = new zxd::DirectionArrow();
    da1->setColor(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));

    da0->setDirection(0, osg::Vec2(1.0f, 0.0f));
    da0->setDirection(1, osg::Vec2(-1.0f, 0.0f));
    da1->setDirection(0, osg::Vec2(0.0f, 1.0f));
    da1->setDirection(1, osg::Vec2(0.0f, -1.0f));

    da0->setMatrix(osg::Matrix::translate(v1));
    da1->setMatrix(osg::Matrix::translate(v1));

    tbHandle->addChild(da0);
    tbHandle->addChild(da1);
    mHandleSwitch->addChild(tbHandle, mMode == 1);
  }

  mHandle->addChild(mHandleSwitch);

  updateHandle();

  OSG_NOTICE << "create scale handle" << std::endl;
}

//------------------------------------------------------------------------------
void RotateOperation::doUpdateXYZ(bool shiftDown) {
  // get handle rotation angle
  osg::Vec3 ab, ac;
  ab[0] = mLastCursor[0] - mWndPivot[0];
  ab[1] = mLastCursor[1] - mWndPivot[1];
  ac[0] = mCurrentCursor[0] - mWndPivot[0];
  ac[1] = mCurrentCursor[1] - mWndPivot[1];

  // check sign
  int sign = (ab ^ ac) * osg::Z_AXIS;
  if (sign == 0) return;  // no rotation
  sign = sign > 0 ? 1 : -1;

  GLfloat dtScale = zxd::angle(ab, ac) * sign;
  dtScale *= shiftDown ? 0.1f : 1.0f;
  mRot += dtScale * mSign;

  rotateObject();
}

//------------------------------------------------------------------------------
void RotateOperation::doUpdateTrackball(bool shiftDown) {
  // yaw pitch object, in view frames without translation

  // osg::Vec2 ndc0(mEa0->getXnormalized(), mEa0->getYnormalized());
  // osg::Vec2 ndc1(mEa1->getXnormalized(), mEa1->getYnormalized());

  // get sphere point
  // osg::Vec3 sp0 = zxd::ndcToSphere(ndc0, mRadius);
  // osg::Vec3 sp1 = zxd::ndcToSphere(ndc1, mRadius);

  // get yaw and pitch to rotate sp0 to sp1. sp0 * pitch * yaw = sp1
  GLfloat scale = shiftDown ? 0.1f : 1.0f;
  osg::Vec2 cursorOffset = mCurrentCursor - mLastCursor;
  mYaw += scale * cursorOffset.x() / 100.0f;
  mPitch += scale * -cursorOffset.y() / 100.0f;

  osg::Matrix matYaw = osg::Matrix::rotate(mYaw, osg::Y_AXIS);
  osg::Matrix matPitch = osg::Matrix::rotate(mPitch, osg::X_AXIS);

  // pitch is applied after yaw, which means it's always "looks right"
  mTarget->setMatrix(
    mModelRS * mViewRS * matYaw * matPitch * mInvViewRS * mModelT);
}

//------------------------------------------------------------------------------
void RotateOperation::updateTextTrackball() {
  if (mInfoText) {
    std::stringstream ss;
    ss.setf(std::ios_base::fixed);
    ss.precision(4);
    ss << "Trackball " << osg::RadiansToDegrees(mPitch) << " "
       << osg::RadiansToDegrees(mYaw);
    mInfoText->setText(ss.str());
  }
}

//------------------------------------------------------------------------------
void RotateOperation::updateTextXYZ() {
  std::string frame = zxd::toString(mAc->getFrame());
  std::stringstream ss;
  ss.setf(std::ios_base::fixed);
  ss.precision(4);

  ss << "Rot " << osg::RadiansToDegrees(mRot) << " ";

  static const char* axes[] = {"X", "Y", "Z"};

  if (mInfoText) {
    switch (mAc->getType()) {
      case zxd::AxisConstrainer::CT_NONE:
        break;
      case zxd::AxisConstrainer::CT_X:
      case zxd::AxisConstrainer::CT_Y:
      case zxd::AxisConstrainer::CT_Z:
        ss << "along " << frame << " "
           << axes[mAc->getType() - zxd::AxisConstrainer::CT_X];
        break;
      case zxd::AxisConstrainer::CT_YZ:
      case zxd::AxisConstrainer::CT_ZX:
      case zxd::AxisConstrainer::CT_XY:
        ss << "locking " << frame << " "
           << axes[mAc->getType() - zxd::AxisConstrainer::CT_YZ];
        break;
    }
    mInfoText->setText(ss.str());
  }
}

//------------------------------------------------------------------------------
void RotateOperation::doApplyAxisConstrainerChange() {
  // get rotation axis
  switch (mAc->getType()) {
    case zxd::AxisConstrainer::CT_NONE:
      mAxis = osg::Matrix::transform3x3(osg::Z_AXIS, mInvView);
      break;
    case zxd::AxisConstrainer::CT_X:
      mAxis = osg::X_AXIS;
      break;
    case zxd::AxisConstrainer::CT_Y:
      mAxis = osg::Y_AXIS;
      break;
    case zxd::AxisConstrainer::CT_Z:
      mAxis = osg::Z_AXIS;
      break;
    case zxd::AxisConstrainer::CT_YZ:
      mAxis = osg::X_AXIS;
      break;
    case zxd::AxisConstrainer::CT_ZX:
      mAxis = osg::Y_AXIS;
      break;
    case zxd::AxisConstrainer::CT_XY:
      mAxis = osg::Z_AXIS;
      break;
    default:
      throw std::runtime_error("unknown constrain type");
  }
  // compute rotation sign, it's determined by dot result of rotation axis and
  // z in view space, it's used to make sure the clockwise of object is the
  // same as handle
  const osg::Matrix m =
    mAc->getFrame() == zxd::AxisConstrainer::CF_LOCAL ? mModelView : mView;
  osg::Vec3 viewAxis = osg::Matrix::transform3x3(mAxis, m);
  float sign = viewAxis * osg::Z_AXIS;

  // if sign is different from original one, change sign of original ortation
  if (sign * mSign < 0) mRot *= -1;
  mSign = sign > .0f ? 1 : -1;
  OSG_NOTICE << "mSign" << mSign << std::endl;

  rotateObject();
}

//------------------------------------------------------------------------------
void RotateOperation::rotateObject() {
  if (mAc->getFrame() == zxd::AxisConstrainer::CF_LOCAL)
    mTarget->setMatrix(mLocalToPivot * osg::Matrix::rotate(mRot, mAxis) *
                       mInvLocalToPivot * mModel);
  else
    mTarget->setMatrix(mModel * mWorldToPivot *
                       osg::Matrix::rotate(mRot, mAxis) * mInvWorldToPivot);
}
}
