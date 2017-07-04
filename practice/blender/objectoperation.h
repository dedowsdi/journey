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
#include "zmath.h"
#include <sstream>
#include "axisconstrainer.h"
#include <osgViewer/View>

namespace zxd {

class Blender;
class Pivot;

// parent class of g s r
class ObjectOperation : public osgGA::GUIEventHandler {
protected:
  osg::Matrix mModel;
  osg::Matrix mInvModel;
  osg::Matrix mModelRS;
  osg::Matrix mInvModelRS;
  osg::Matrix mModelT;
  osg::Matrix mViewProj;
  osg::Matrix mViewProjWnd;
  osg::Matrix mInvViewProjWnd;
  osg::Matrix mWnd;
  osg::Matrix mInvWnd;
  osg::Matrix mView;
  osg::Matrix mInvView;
  osg::Matrix mViewRS;
  osg::Matrix mInvViewRS;
  osg::Matrix mModelView;
  osg::Matrix mInvModelView;
  osg::Matrix mModelViewProjWnd;
  osg::Matrix mInvModelViewProjWnd;
  osg::Matrix mProj;
  osg::Matrix mInvProj;
  osg::Matrix mProjWnd;
  osg::Matrix mInvProjWnd;

  osg::Vec2 mStartCursor;
  osg::Vec2 mLastCursor;
  osg::Vec2 mCurrentCursor;
  osg::Vec3 mWndStartObject;
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
  osg::Camera* mCamera;
  Pivot* mPivot;
  osg::Vec2 mWndPivot;        // operation handle like scale or rotate need this
  osg::Vec3 mWorldPivot;      // pivot in world space
  osg::Vec3 mLocalPivot;      // pivot in initial local space
  osg::Matrix mWorldToPivot;  // world to world pivot
  osg::Matrix mInvWorldToPivot;  // world to world pivot
  osg::Matrix mLocalToPivot;     // local to local pivot
  osg::Matrix mInvLocalToPivot;  // local to local pivot

  osgViewer::View* mBlenderView;

  bool mOrtho;  // whether current camera proj is ortho

protected:
  osg::ref_ptr<AxisConstrainer> mAc;

public:
  ObjectOperation() {}

  // ObjectOperation(const ObjectOperation& copy,
  // const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY)
  //: osg::GUIEventHandler(copy, copyop) {}
  //~ObjectOperation() {}
  // META_Object(zxd, ObjectOperation);

  virtual void init(osgViewer::View* view, osg::Vec2 v);

  virtual osg::Node* getHandle() { return 0; }

  AxisConstrainer* getAxisConstrainer() const { return mAc; }
  void setAxisConstrainer(AxisConstrainer* v);

  osg::ref_ptr<osgText::Text> getInfoText() const { return mInfoText; }
  void setInfoText(osg::ref_ptr<osgText::Text> v) { mInfoText = v; }

  // reset transform
  void cancel();
  // confirm transform
  void confirm() { clean(); }
  // x y in window frame
  virtual void update(bool shiftDown);

  inline osg::Vec2 getCurrentCursor() const { return mCurrentCursor; }
  inline void setCurrentCursor(osg::Vec2 v) { mCurrentCursor = v; }

  osg::ref_ptr<zxd::BlenderObject> getTarget() const { return mTarget; }
  void setTarget(osg::ref_ptr<zxd::BlenderObject> v);

  Pivot* getPivot() const { return mPivot; }
  void setPivot(Pivot* v);

  virtual const std::string& getType() {
    static std::string s("blank");
    return s;
  };

  osg::Camera* getCamera() const { return mCamera; }
  void setCamera(osg::Camera* v) { mCamera = v; }

  osgViewer::View* getBlenderView() const { return mBlenderView; }
  void setBlenderView(osgViewer::View* v) { mBlenderView = v; }

  bool getOrtho() const { return mOrtho; }

protected:
  virtual bool handle(
    const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

  virtual void applyAxisConstrainerChange() {
    doApplyAxisConstrainerChange();
    retransfromAxisConstrainer();
  }
  virtual void doApplyAxisConstrainerChange() {}
  virtual void onShiftDown() {}
  virtual void doUpdate(bool shiftDown) {}
  virtual void updateText() {}
  virtual void updateHandle() {}

  virtual void doInit() {}
  void retransfromAxisConstrainer();

  // clean help stuff: like handle during scale
  virtual void clean();

  void getHandlePosition(osg::Vec3& v0, osg::Vec3& v1) {
    v0 = osg::Vec3(osg::Vec3(mWndPivot.x(), mWndPivot.y(), 0));
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

public:
  virtual const std::string& getType() {
    static std::string s = "grab";
    return s;
  };

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
    return osg::Vec2(mWndStartObject[0], mWndStartObject[1]) + cursorOffset;
  }

  void getCameraRay(osg::Vec3& rayStart, osg::Vec3& dir) {
    osg::Vec3 p0, p1;
    const osg::Matrix* invMat =
      mAc->getFrame() == zxd::AxisConstrainer::CF_LOCAL ? &mInvModelViewProjWnd
                                                        : &mInvViewProjWnd;

    zxd::Math::getCameraRay(mCurrentCursor,  *invMat, p0, p1);

    rayStart = p0;
    dir = p1 - p0;
    dir.normalize();
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

  osg::Plane getConstrainPlane();
  void getConstrainPlaneAxes(osg::Vec3& v0, osg::Vec3& v1);

  osg::Vec3 getConstrainAxis();

  // get final offset in local or world space
  void computeOffset(bool shiftDown);

  osg::Vec3 cameraRayTestSingleAxis(
    const osg::Vec3& rayStart, const osg::Vec3& dir, const osg::Vec3& axis);

  virtual void doUpdate(bool shiftDown);

  // get accumulated offset in curent constrained frame, it's not the same as
  // mOffset.
  osg::Vec3 getAccumulatedOffset();

  virtual void updateText();
  virtual void doInit() {}

  virtual void doApplyAxisConstrainerChange();

  virtual void translateObject();
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
  virtual const std::string& getType() {
    static std::string s = "scale";
    return s;
  };
  virtual void doInit();
  virtual osg::Node* getHandle() { return mHandle; }

protected:
  void updateHandle();
  virtual void doUpdate(bool shiftDown);

  osg::Vec3 getScaleVector();

  // scale according to current scale factor and axis constrainer
  virtual void scaleObject();

  virtual void doApplyAxisConstrainerChange() { scaleObject(); }

  virtual void updateText();
};
class RotateOperation : public ObjectOperation {
protected:
  GLfloat mRot;  // accumulated rotation in xyz mode
  osg::Vec3 mAxis;
  osg::Vec2 mTrackballCursor;  // cursor position when enabling trackball
  osg::ref_ptr<osg::Group> mHandle;
  osg::ref_ptr<osg::Switch> mHandleSwitch;
  GLfloat mSign;
  int mMode;        // 0 as xyz, 1 as trackball
  GLfloat mRadius;  // trackball radius
  GLfloat mYaw, mPitch;

public:
  RotateOperation()
      : mRot(0),
        mSign(1.0f),
        mMode(0),
        mRadius(0.9f),
        mYaw(0.0f),
        mPitch(0.0f) {}
  virtual const std::string& getType() {
    static std::string s = "rotate";
    return s;
  };
  virtual osg::Node* getHandle() { return mHandle; }

  GLfloat getRadius() const { return mRadius; }
  void setRadius(GLfloat v) { mRadius = v; }

  int getMode() const { return mMode; }
  void setMode(int v);

  void toggleMode() { setMode(mMode ^ 1); }

protected:
  void updateHandle() {
    if (mMode == 0)
      updateHandleXYZ();
    else
      updateHandleTrackball();
  }
  void updateHandleTrackball();
  void updateHandleXYZ();

  virtual void doInit();
  virtual void doUpdate(bool shiftDown) {
    if (mMode == 0)
      doUpdateXYZ(shiftDown);
    else
      doUpdateTrackball(shiftDown);
  }
  virtual void doUpdateXYZ(bool shiftDown);

  virtual void doUpdateTrackball(bool shiftDown);
  virtual void updateText() {
    if (mMode == 0)
      updateTextXYZ();
    else
      updateTextTrackball();
  }
  virtual void updateTextTrackball();

  virtual void updateTextXYZ();
  virtual void doApplyAxisConstrainerChange();

  void rotateObject();
};
}

#endif /* OBJECTOPERATION_H */
