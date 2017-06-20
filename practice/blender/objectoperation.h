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

  osg::ref_ptr<osgText::Text> mInfoText;

  GLfloat mNear;

  osg::ref_ptr<zxd::BlenderObject> mTarget;

protected:
  osg::ref_ptr<AxisConstrainer> mAxisConstrainer;

  virtual bool handle(
    const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) {
    bool shiftDown = ea.getModKeyMask() & osgGA::GUIEventAdapter::MODKEY_SHIFT;
    switch (ea.getEventType()) {
      case osgGA::GUIEventAdapter::KEYDOWN:
        switch (ea.getUnmodifiedKey()) {
          case osgGA::GUIEventAdapter::KEY_X:
            mAxisConstrainer->addConstrain(
              shiftDown ? AxisConstrainer::CT_YZ : AxisConstrainer::CT_X);
            applyAxisConstrainerChange();
            break;
          case osgGA::GUIEventAdapter::KEY_Y:
            mAxisConstrainer->addConstrain(
              shiftDown ? AxisConstrainer::CT_ZX : AxisConstrainer::CT_Y);
            applyAxisConstrainerChange();
            break;
          case osgGA::GUIEventAdapter::KEY_Z:
            mAxisConstrainer->addConstrain(
              shiftDown ? AxisConstrainer::CT_XY : AxisConstrainer::CT_Z);
            applyAxisConstrainerChange();
            break;
          default:
            break;
        }
        break;
      case osgGA::GUIEventAdapter::MOVE: {
        setCurrentCursor(osg::Vec2(ea.getX(), ea.getY()));
        update(ea.getModKeyMask() & osgGA::GUIEventAdapter::MODKEY_LEFT_SHIFT);
        mLastCursor = mCurrentCursor;
      } break;

      default:
        break;
    }
    return false;  // return true will stop event
  }

  virtual void applyAxisConstrainerChange() {}

public:
  // ObjectOperation() {}

  // ObjectOperation(const ObjectOperation& copy,
  // const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY)
  //: osg::GUIEventHandler(copy, copyop) {}
  //~ObjectOperation() {}
  // META_Object(zxd, ObjectOperation);

  AxisConstrainer* getAxisConstrainer() const { return mAxisConstrainer; }
  void setAxisConstrainer(AxisConstrainer* v) { mAxisConstrainer = v; }

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
  }

  inline osg::Vec2 getCurrentCursor() const { return mCurrentCursor; }
  inline void setCurrentCursor(osg::Vec2 v) { mCurrentCursor = v; }

  virtual void doUpdate(bool shiftDown) {}
  virtual void updateText() {}

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

    mCurrentCursor = v;
    mStartCursor = v;

    mNearPlaneStartCursor =
      osg::Vec3(mStartCursor.x(), mStartCursor.y(), 0) * mInvProjWnd;

    doInit();
  }
  virtual void doInit() {}

  osg::ref_ptr<zxd::BlenderObject> getTarget() const { return mTarget; }
  void setTarget(osg::ref_ptr<zxd::BlenderObject> v) {
    mTarget = v;
    if (mAxisConstrainer) mAxisConstrainer->setTarget(v);
  }

  // clean help stuff: like handle during scale
  virtual void clean() {
    // remove axis constrainer and handle from scene
    zxd::removeNodeParents(mAxisConstrainer);
    OSG_NOTICE << "clear axis constrainer" << std::endl;

    osg::Node* handle = getHandle();
    if (handle) {
      zxd::removeNodeParents(handle);
      OSG_NOTICE << "clear handle" << std::endl;
    }
  }
  virtual osg::Node* getHandle() { return 0; }

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
 */
class GrabOperation : public ObjectOperation {
protected:
  osg::Vec3 mOffset;  // current offset

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
    osg::Matrix* invMatrix =
      mAxisConstrainer->getConstrainFrame() == zxd::AxisConstrainer::CF_LOCAL
        ? &mInvModelViewProjWnd
        : &mInvViewProjWnd;

    return osg::Vec3(wndPos.x(), wndPos.y(), 0) * *invMatrix;
  }

  inline osg::Vec3 getRayStart() {
    return mAxisConstrainer->getConstrainFrame() ==
               zxd::AxisConstrainer::CF_LOCAL
             ? mModelCameraPos
             : mCameraPos;
  }

  inline osg::Vec3 getObjectStart() {
    return mAxisConstrainer->getConstrainFrame() ==
               zxd::AxisConstrainer::CF_LOCAL
             ? osg::Vec3()
             : mWorldStartObject;
  }

  osg::Plane getConstrainPlane() {
    osg::Vec4 pv;

    osg::Vec3 v;
    switch (mAxisConstrainer->getConstrainType()) {
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

    if (mAxisConstrainer->getConstrainFrame() ==
        zxd::AxisConstrainer::CF_LOCAL) {
      return osg::Plane(v, 0);
    } else {
      return osg::Plane(v, mWorldStartObject);
    }
  }

  // always use world constrain axis
  osg::Vec3 getConstrainAxis() {
    osg::Vec3 v;
    switch (mAxisConstrainer->getConstrainType()) {
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
  void computeOffset() {
    switch (mAxisConstrainer->getConstrainType()) {
      case zxd::AxisConstrainer::CT_NONE: {
        // get near plane final projection place, than scale to get view
        // position
        osg::Vec2 wndEndPos = getWndEndPos();
        osg::Vec3 npEndPos =
          osg::Vec3(wndEndPos.x(), wndEndPos.y(), 0) * mInvProjWnd;
        osg::Vec3 viewEndPos = npEndPos * (mViewStartObject.z() / -mNear);
        mOffset =
          osg::Matrix::transform3x3(viewEndPos - mViewStartObject, mInvView);
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

        // if camera ray and constrain axis parallel to each other, clear
        // translation
        mOffset = res.first ? res.second.second - objStart : osg::Vec3()
      } break;

      case zxd::AxisConstrainer::CT_YZ:
      case zxd::AxisConstrainer::CT_ZX:
      case zxd::AxisConstrainer::CT_XY: {
        // get camra ray in world or local frame, interset with constrained
        osg::Vec2 wndEndPos = getWndEndPos();
        osg::Vec3 rayStart = getRayStart();
        osg::Vec3 rayEnd = getRayEnd(wndEndPos);
        osg::Vec3 objStart = getObjectStart();

        osg::Vec3 dir = rayEnd - rayStart;
        dir.normalize();

        osg::Plane constrainPlane = getConstrainPlane();
        // find intersection between ray and constrain plane
        zxd::LinePlaneRelPair res = zxd::Math::getLinePlaneRelation(
          mCameraPos, dir, constrainPlane.asVec4());

        // if camera ray and constrain plane parallel to each other, clear
        // translation
        mOffset = res.first ? res.second - objStart : osg::Vec3()

      } break;
      default:
        throw std::runtime_error("unknown constrain type");
    }
  }

  virtual void doUpdate(bool shiftDown) {
    computeOffset();
    translateObject();
  }

  virtual void updateText() {
    std::string frame = zxd::toString(mAxisConstrainer->getConstrainFrame());
    std::stringstream ss;
    ss.precision(4);
    ss.setf(std::ios_base::fixed);
    float l = mOffset.length();

    static const char* axes[] = {"X", "Y", "Z"};

    if (mInfoText) {
      switch (mAxisConstrainer->getConstrainType()) {
        case zxd::AxisConstrainer::CT_NONE:
          ss << "Dx:" << mOffset.x() << " Dy:" << mOffset.y()
             << " Dz:" << mOffset.z() << " (" << l << ")";
          break;
        case zxd::AxisConstrainer::CT_X:
        case zxd::AxisConstrainer::CT_Y:
        case zxd::AxisConstrainer::CT_Z: {
          int idx =
            mAxisConstrainer->getConstrainType() - zxd::AxisConstrainer::CT_X;
          ss << "D:" << mOffset[idx] << " (" << l << ") along " << frame << " "
             << axes[idx];
        } break;
        case zxd::AxisConstrainer::CT_YZ:
        case zxd::AxisConstrainer::CT_ZX:
        case zxd::AxisConstrainer::CT_XY: {
          int idx =
            mAxisConstrainer->getConstrainType() - zxd::AxisConstrainer::CT_X;
          ss << "D:" << mOffset[(idx + 1) % 3]
             << " D:" << mOffset[(idx + 2) % 3] << " (" << l << ") locking "
             << frame << " " << axes[idx];
        } break;
      }

      mInfoText->setText(ss.str());
    }
  }
  virtual void doInit() {}

  virtual void applyAxisConstrainerChange() {
    computeOffset();
    translateObject();
  }

  virtual void translateObject() {
    if (mAxisConstrainer->getConstrainFrame() == zxd::AxisConstrainer::CF_LOCAL)
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
  float mScale;  // used to store current operation scale
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
    lsn->setPosition(v0, v1);

    osg::StateSet* ss = lsn->getOrCreateStateSet();
    ss->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
    ss->setAttributeAndModes(new osg::LineStipple(1.0f, 0xF0F0));

    // position direction arrow at cursor place
    osg::ref_ptr<zxd::DirectionArrow> da = new zxd::DirectionArrow();
    da->setMatrix(osg::Matrix::translate(v1));
    da->setDirection(0, mCurrentCursor - mWndStartObject);
    da->setDirection(1, mWndStartObject - mCurrentCursor);

    mHandle->addChild(lsn);
    mHandle->addChild(da);

    OSG_NOTICE << "create scale handle" << std::endl;
  }
  virtual osg::Node* getHandle() { return mHandle; }

protected:
  virtual void doUpdate(bool shiftDown) {
    // get scale change
    GLfloat curHandleLength = (mCurrentCursor - mWndStartObject).length();
    GLfloat dtScale =
      (curHandleLength - mLastHandleLength) / mInitalHandleLength;
    mLastHandleLength = curHandleLength;

    mScale += shiftDown ? dtScale * 0.1f : dtScale;

    scaleObject();

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

  osg::Vec3 getScaleVector() {
    osg::Vec3 scaleVector;
    switch (mAxisConstrainer->getConstrainType()) {
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

    if (mAxisConstrainer->getConstrainFrame() == AxisConstrainer::CF_LOCAL)
      mTarget->setMatrix(scaleMatrix * mModel);
    else
      mTarget->setMatrix(mModel * scaleMatrix);
  }

  virtual void applyAxisConstrainerChange() { scaleObject(); }

  virtual void updateText() {
    std::string frame = zxd::toString(mAxisConstrainer->getConstrainFrame());
    std::stringstream ss;
    ss << "Scale ";
    ss.precision(4);
    ss.setf(std::ios_base::fixed);

    static const char* axes[] = {"X", "Y", "Z"};

    if (mInfoText) {
      switch (mAxisConstrainer->getConstrainType()) {
        case zxd::AxisConstrainer::CT_NONE:
          ss << "X:" << mScale << " Y:" << mScale << " Z:" << mScale;
          break;
        case zxd::AxisConstrainer::CT_X:
        case zxd::AxisConstrainer::CT_Y:
        case zxd::AxisConstrainer::CT_Z:
          ss << mScale << " along " << frame << " "
             << axes[mAxisConstrainer->getConstrainType() -
                     zxd::AxisConstrainer::CT_X];
          break;
        case zxd::AxisConstrainer::CT_YZ:
        case zxd::AxisConstrainer::CT_ZX:
        case zxd::AxisConstrainer::CT_XY:
          ss << mScale << " : " << mScale << " locking " << frame << " "
             << axes[mAxisConstrainer->getConstrainType() -
                     zxd::AxisConstrainer::CT_XY];
          break;
      }

      mInfoText->setText(ss.str());
    }
  }
};
class RotateOperation : public ObjectOperation {};
}

#endif /* OBJECTOPERATION_H */
