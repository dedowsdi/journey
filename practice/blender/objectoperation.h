#ifndef OBJECTOPERATION_H
#define OBJECTOPERATION_H

#include <osgGA/GUIEventHandler>
#include "axisconstrainer.h"
#include "auxiliary.h"
#include "common.h"
#include <osg/LineStipple>
#include <osg/io_utils>
#include "zmath.h"
#include <iomanip>

namespace zxd {

// parent class of g s r
class ObjectOperation : public osgGA::GUIEventHandler {
protected:
  osg::Matrix mModel;
  osg::Matrix mInvModel;
  osg::Matrix mViewProj;
  osg::Matrix mViewProjWnd;
  osg::Matrix mInvViewProjWnd;
  osg::Matrix mWnd;
  osg::Matrix mInvWnd;
  osg::Matrix mView;
  osg::Matrix mInvView;
  osg::Matrix mModelView;
  osg::Matrix mInvModelView;
  osg::Matrix mModelViewProjWnd;
  osg::Matrix mInvModelViewProjWnd;
  osg::Matrix mProj;
  osg::Matrix mInvProj;
  osg::Matrix mProjWnd;
  osg::Matrix mInvProjWnd;
  osg::Matrix mPivot;  // TODO

  osg::Vec2 mStartCursor;
  osg::Vec2 mWndStartObject;
  osg::Vec2 mLastCursor;
  osg::Vec2 mCurrentCursor;
  osg::Vec3 mViewStartObject;  // object position in view
  osg::Vec3 mWorldStartObject;
  osg::Vec3 mNearPlaneStartCursor;
  osg::Vec3 mNearPlaneStartObject;
  osg::Vec3 mCameraPos;
  osg::Vec3 mModelCameraPos;

  osg::ref_ptr<const osgGA::GUIEventAdapter> mEa0;  // last
  osg::ref_ptr<const osgGA::GUIEventAdapter> mEa1;  // current

  osg::ref_ptr<osgText::Text> mInfoText;

  GLfloat mNear;

  osg::ref_ptr<zxd::BlenderObject> mTarget;

protected:
  osg::ref_ptr<AxisConstrainer> mAc;

public:
  // ObjectOperation() {}

  // ObjectOperation(const ObjectOperation& copy,
  // const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY)
  //: osg::GUIEventHandler(copy, copyop) {}
  //~ObjectOperation() {}
  // META_Object(zxd, ObjectOperation);

  virtual void init(osg::Camera* camera, osg::Vec2 v) {
    mModel = mTarget->getMatrix();
    mInvModel.invert(mModel);

    mView = camera->getViewMatrix();
    mInvView.invert(mView);

    mCameraPos = mInvView.getTrans();

    mWnd = camera->getViewport()->computeWindowMatrix();
    mInvWnd.invert(mWnd);

    mProj = camera->getProjectionMatrix();
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

    GLfloat left, right, bottom, top, far;
    mProj.getFrustum(left, right, bottom, top, mNear, far);
    OSG_NOTICE << "left:" << left << " right :" << right << std::endl;

    mViewStartObject = mModel.getTrans() * mView;
    mNearPlaneStartObject =
      mViewStartObject * (-mNear / mViewStartObject.z());  // scale to near

    mCurrentCursor = mStartCursor = mLastCursor = v;

    mNearPlaneStartCursor =
      osg::Vec3(mStartCursor.x(), mStartCursor.y(), 0) * mInvProjWnd;

    doInit();
    applyAxisConstrainerChange();
  }

  virtual osg::Node* getHandle() { return 0; }

  AxisConstrainer* getAxisConstrainer() const { return mAc; }
  void setAxisConstrainer(AxisConstrainer* v) { mAc = v; }

  osg::ref_ptr<osgText::Text> getInfoText() const { return mInfoText; }
  void setInfoText(osg::ref_ptr<osgText::Text> v) { mInfoText = v; }

  // reset transform
  void cancel() {
    mTarget->setMatrix(mModel);
    clean();
  }
  // confirm transform
  void confirm() { clean(); }
  // x y in window frame
  virtual void update(bool shiftDown) {
    doUpdate(shiftDown);
    updateText();
    updateHandle();
  }

  inline osg::Vec2 getCurrentCursor() const { return mCurrentCursor; }
  inline void setCurrentCursor(osg::Vec2 v) { mCurrentCursor = v; }

  osg::ref_ptr<zxd::BlenderObject> getTarget() const { return mTarget; }
  void setTarget(osg::ref_ptr<zxd::BlenderObject> v) {
    mTarget = v;
    if (mAc) mAc->setTarget(v);
  }

protected:
  virtual bool handle(
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

  virtual void applyAxisConstrainerChange() {}
  virtual void onShiftDown() {}
  virtual void doUpdate(bool shiftDown) {}
  virtual void updateText() {}
  virtual void updateHandle() {}

  virtual void doInit() {}

  // clean help stuff: like handle during scale
  virtual void clean() {
    // remove axis constrainer and handle from scene
    zxd::removeNodeParents(mAc);
    OSG_NOTICE << "clear axis constrainer" << std::endl;

    osg::Node* handle = getHandle();
    if (handle) {
      zxd::removeNodeParents(handle);
      OSG_NOTICE << "clear handle" << std::endl;
    }
  }

  void getHandlePosition(osg::Vec3& v0, osg::Vec3& v1) {
    v0 = osg::Vec3(osg::Vec3(mWndStartObject.x(), mWndStartObject.y(), 0));
    v1 = osg::Vec3(osg::Vec3(mCurrentCursor.x(), mCurrentCursor.y(), 0));
  }
};

/*
 * grab:
 *   no constrain:
 *     grabing along plane which perp to z and contain init position.
 *     cursor and obj wnd relative pos remain still
 *   double constrain:
 *     grabing along target plane
 *     cursor and obj wnd relative pos remain still
 *   single constrain:
 *     grabing along target axes
 *     cursor and obj wnd relative pos remain still on certain axis
 *
 * issue
 *    if you stop pressing shift, in order to keep window relative pos between
 *    cursor and object, there will be a sudden jump, just like blender.
 */
class GrabOperation : public ObjectOperation {
protected:
  osg::Vec3 mOffset;         // accumulated offset
  osg::Vec3 mOffsetShift;    // recorded current offset when shift down
  osg::Matrix mInvCurModel;  // inverse of current model

protected:
  osg::Vec3 getNearPlaneOffset() {
    // get proj on near plane
    osg::Vec3 npCurrentCursor =
      osg::Vec3(mCurrentCursor.x(), mCurrentCursor.y(), 0) * mInvProjWnd;
    // get cursor offset on near plane, it's also the object offset on near
    // plane
    return npCurrentCursor - mNearPlaneStartCursor;
  }

  osg::Vec2 getWndEndPos() {
    osg::Vec2 cursorOffset = mCurrentCursor - mStartCursor;
    return mWndStartObject + cursorOffset;
  }

  // get ray end in world or local space
  inline osg::Vec3 getRayEnd(const osg::Vec2& wndPos) {
    // get world ray end
    osg::Vec3 end = osg::Vec3(wndPos.x(), wndPos.y(), 0) * mInvViewProjWnd;

    // transform back to local if necessary, becareful, world is changing
    return mAc->getFrame() == zxd::AxisConstrainer::CF_LOCAL
             ? end * mInvCurModel
             : end;
  }

  inline osg::Vec3 getRayStart() {
    return mAc->getFrame() == zxd::AxisConstrainer::CF_LOCAL
             ? mCameraPos * mInvCurModel
             : mCameraPos;
  }

  inline osg::Vec3 getObjectStart() {
    return mAc->getFrame() == zxd::AxisConstrainer::CF_LOCAL
             ? osg::Vec3()
             : mWorldStartObject;
  };

  inline osg::Vec3 getObjectCurrent() {
    return mAc->getFrame() == zxd::AxisConstrainer::CF_LOCAL
             ? osg::Vec3()
             : osg::Vec3(mTarget->getMatrix().getTrans());
  }
  virtual void onShiftDown() { mOffsetShift = mOffset; }

  osg::Plane getConstrainPlane() {
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

  osg::Vec3 getConstrainAxis() {
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

  // get final offset in local or world space
  void computeOffset(bool shiftDown) {
    osg::Vec3 endPos;  // endPos in local or world
    switch (mAc->getType()) {
      case zxd::AxisConstrainer::CT_NONE: {
        // get near plane final projection place, than scale to get view
        // position
        osg::Vec2 wndEndPos = getWndEndPos();
        osg::Vec3 npEndPos =
          osg::Vec3(wndEndPos.x(), wndEndPos.y(), 0) * mInvProjWnd;
        osg::Vec3 viewEndPos = npEndPos * (mViewStartObject.z() / -mNear);
        endPos = viewEndPos * mInvView;
      } break;

      case zxd::AxisConstrainer::CT_X:
      case zxd::AxisConstrainer::CT_Y:
      case zxd::AxisConstrainer::CT_Z: {
        // get camera ray in world or local frame, skew with constrained axis
        osg::Vec2 wndEndPos = getWndEndPos();
        osg::Vec3 rayStart = getRayStart();
        osg::Vec3 rayEnd = getRayEnd(wndEndPos);
        osg::Vec3 objStart = getObjectStart();

        osg::Vec3 dir = rayEnd - rayStart;
        dir.normalize();

        // get target world and wnd axis
        osg::Vec3 constrainAxis = getConstrainAxis();

        zxd::LineRelPair res =
          zxd::Math::getLineRelation(rayStart, dir, objStart, constrainAxis);

        // if camera ray and constrain axis parallel to each other, just clear
        // translation
        if (res.first)
          endPos = res.second.second;
        else
          endPos = mTarget->getMatrix().getTrans();
      } break;

      case zxd::AxisConstrainer::CT_YZ:
      case zxd::AxisConstrainer::CT_ZX:
      case zxd::AxisConstrainer::CT_XY: {
        // get camra ray in world or local frame, interset with constrained
        osg::Vec2 wndEndPos = getWndEndPos();
        osg::Vec3 rayStart = getRayStart();
        osg::Vec3 rayEnd = getRayEnd(wndEndPos);

        osg::Vec3 dir = rayEnd - rayStart;
        dir.normalize();

        osg::Plane constrainPlane = getConstrainPlane();
        // find intersection between ray and constrain plane
        zxd::LinePlaneRelPair res = zxd::Math::getLinePlaneRelation(
          mCameraPos, dir, constrainPlane.asVec4());

        // if camera ray and constrain plane parallel to each other, clear
        // translation
        if (res.first)
          endPos = res.second;
        else
          endPos = mTarget->getMatrix().getTrans();

      } break;
      default:
        throw std::runtime_error("unknown constrain type");
    }

    mOffset = endPos - getObjectStart();
    if (shiftDown) {
      mOffset = mOffsetShift + (mOffset - mOffsetShift) * 0.1f;
    }
  }

  virtual void doUpdate(bool shiftDown) {
    mInvCurModel.invert(mTarget->getMatrix());
    computeOffset(shiftDown);
    translateObject();
  }

  // get accumulated offset in curent constrained frame, it's not the same as
  // mOffset.
  osg::Vec3 getAccumulatedOffset() {
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

  virtual void updateText() {
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
  virtual void doInit() {}

  virtual void applyAxisConstrainerChange() {
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
    std::for_each(
      axes->begin(), axes->end(), [&](decltype(*axes->begin()) axis) {
        mOffset = mOffset + axis * (offset * axis);
      });

    // get new offsetShift
    if (offsetShiftRatio != 0.0f) mOffsetShift = mOffset * offsetShiftRatio;

    translateObject();
  }

  virtual void translateObject() {
    if (mAc->getFrame() == zxd::AxisConstrainer::CF_LOCAL)
      mTarget->setMatrix(osg::Matrix::translate(mOffset) * mModel);
    else
      mTarget->setMatrix(mModel * osg::Matrix::translate(mOffset));
  }
};

/*
 * scale:
 *   scale according to handle length
 *  i have to zoom camera before scaling y, otherwise target is invisible,
 *  don't know y.
 */
class ScaleOperation : public ObjectOperation {
protected:
  float mInitalHandleLength;
  float mLastHandleLength;
  float mScale;  // accumulated scale
  osg::ref_ptr<osg::Group> mHandle;

public:
  ScaleOperation() : ObjectOperation(), mScale(1.0f) {}
  virtual void doInit() {
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
  virtual osg::Node* getHandle() { return mHandle; }

protected:
  void updateHandle() {
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
    da->setDirection(0, mCurrentCursor - mWndStartObject);
    da->setDirection(1, mWndStartObject - mCurrentCursor);
  }
  virtual void doUpdate(bool shiftDown) {
    // get scale change
    GLfloat curHandleLength = (mCurrentCursor - mWndStartObject).length();
    GLfloat dtScale =
      (curHandleLength - mLastHandleLength) / mInitalHandleLength;
    mLastHandleLength = curHandleLength;

    mScale += shiftDown ? dtScale * 0.1f : dtScale;

    scaleObject();
  }

  osg::Vec3 getScaleVector() {
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

  // scale according to current scale factor and axis constrainer
  virtual void scaleObject() {
    osg::Vec3 scaleVector = getScaleVector();
    osg::Matrix scaleMatrix = osg::Matrix::scale(scaleVector);

    if (mAc->getFrame() == AxisConstrainer::CF_LOCAL)
      mTarget->setMatrix(scaleMatrix * mModel);
    else
      mTarget->setMatrix(mModel * scaleMatrix);
  }

  virtual void applyAxisConstrainerChange() { scaleObject(); }

  virtual void updateText() {
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
};
class RotateOperation : public ObjectOperation {
protected:
  GLfloat mRot;  // accumulated rotation
  osg::Vec3 mAxis;
  osg::Vec2 mPivot;  // handle window pivot
  osg::ref_ptr<osg::Group> mHandle;
  GLfloat mSign;
  int mMode;        // 0 as xyz, 1 as trackball
  GLfloat mRadius;  // trackball radius
  GLfloat mYaw, mPitch;

public:
  RotateOperation()
      : mSign(1.0f), mMode(0), mRadius(0.9f), mYaw(0.0f), mPitch(0.0f) {}
  virtual osg::Node* getHandle() { return mHandle; }

  GLfloat getRadius() const { return mRadius; }
  void setRadius(GLfloat v) { mRadius = v; }

  int getMode() const { return mMode; }
  void setMode(int v) {
    mMode = v;
    // clear transform during switch
    mTarget->setMatrix(mModel);
    if (mMode == 0) applyAxisConstrainerChange();
  }

protected:
  void updateHandle() {
    zxd::LineSegmentNode* lsn =
      static_cast<zxd::LineSegmentNode*>(mHandle->getChild(0));
    zxd::DirectionArrow* da =
      static_cast<zxd::DirectionArrow*>(mHandle->getChild(1));

    // update handle line
    osg::Vec3 v0, v1;
    getHandlePosition(v0, v1);
    lsn->setEndPosition(v1);

    // upate handle arrow direction, which perps to handle line
    osg::Vec2 r = mCurrentCursor - mPivot;
    osg::Vec2 dir(-r[1], r[0]);
    da->setDirection(0, dir);
    da->setDirection(1, -dir);
    da->setMatrix(osg::Matrix::translate(v1));
  }

  virtual void doInit() {
    mPivot = mWndStartObject;

    // create handle
    mHandle = new osg::Group;

    // handle line
    osg::ref_ptr<zxd::LineSegmentNode> lsn = new zxd::LineSegmentNode();
    osg::Vec3 v0, v1;
    getHandlePosition(v0, v1);
    lsn->setPosition(v0, v1);

    osg::StateSet* ss = lsn->getOrCreateStateSet();
    ss->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
    ss->setAttributeAndModes(new osg::LineStipple(1.0f, 0xF0F0));

    // position direction arrow at cursor place
    osg::ref_ptr<zxd::DirectionArrow> da = new zxd::DirectionArrow();
    da->setMatrix(osg::Matrix::translate(v1));

    mHandle->addChild(lsn);
    mHandle->addChild(da);

    updateHandle();

    OSG_NOTICE << "create scale handle" << std::endl;
  }
  virtual void doUpdate(bool shiftDown) {
    if (mMode == 0)
      doUpdateXYZ(shiftDown);
    else
      doUpdateTrackball(shiftDown);
  }
  virtual void doUpdateXYZ(bool shiftDown) {
    // get handle rotation angle
    osg::Vec3 ab, ac;
    ab[0] = mLastCursor[0] - mPivot[0];
    ab[1] = mLastCursor[1] - mPivot[1];
    ac[0] = mCurrentCursor[0] - mPivot[0];
    ac[1] = mCurrentCursor[1] - mPivot[1];

    // check sign
    int sign = (ab ^ ac) * osg::Z_AXIS;
    if (sign == 0) return;  // no rotation
    sign = sign > 0 ? 1 : -1;

    GLfloat dtScale = zxd::angle(ab, ac) * sign;
    dtScale *= shiftDown ? 0.1f : 1.0f;
    mRot += dtScale * mSign;

    rotateObject();
  }

  virtual void doUpdateTrackball(bool shiftDown) {
    osg::Vec2 ndc0(mEa0->getXnormalized(), mEa0->getYnormalized());
    osg::Vec2 ndc1(mEa1->getXnormalized(), mEa1->getYnormalized());

    // get sphere point
    osg::Vec3 sp0 = zxd::ndcToSphere(ndc0, mRadius);
    osg::Vec3 sp1 = zxd::ndcToSphere(ndc1, mRadius);

    // get yaw and pitch to rotate sp0 to sp1. sp0 * pitch * yaw = sp1
  }
  virtual void updateText() {
    if (mMode == 0)
      updateTextXYZ();
    else
      updateTextTrackball();
  }
  virtual void updateTextTrackball() {}

  virtual void updateTextXYZ() {
    std::string frame = zxd::toString(mAc->getFrame());
    std::stringstream ss;
    ss.setf(std::ios_base::fixed);
    ss.precision(4);

    ss << "Rot " << mRot << " ";

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
  virtual void applyAxisConstrainerChange() {
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

  void rotateObject() {
    if (mAc->getFrame() == zxd::AxisConstrainer::CF_LOCAL)
      mTarget->setMatrix(osg::Matrix::rotate(mRot, mAxis) * mModel);
    else
      mTarget->setMatrix(mModel * osg::Matrix::rotate(mRot, mAxis));
  }
};
}

#endif /* OBJECTOPERATION_H */
