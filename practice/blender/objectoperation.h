#ifndef OBJECTOPERATION_H
#define OBJECTOPERATION_H

#include <osgGA/GUIEventHandler>
#include "axisconstrainer.h"
#include "auxiliary.h"

namespace zxd {

// parent class of g s r
class ObjectOperation : public osgGA::GUIEventHandler {
protected:
  osg::Matrix mModel;
  osg::Matrix mViewProj;
  osg::Matrix mViewProjWnd;
  osg::Matrix mWnd;
  osg::Matrix mInvWnd;
  osg::Matrix mView;
  osg::Matrix mInvView;
  osg::Matrix mProj;
  osg::Matrix mInvProj;
  osg::Matrix mProjWnd;
  osg::Matrix mInvProjWnd;
  osg::Matrix mPivot;  // TODO

  osg::Vec2 mStartWindowPos;
  osg::Vec2 mLastCursor;
  osg::Vec2 mCurrentCursor;
  osg::Vec2 mStartCursor;

  osg::ref_ptr<zxd::BlenderObject> mTarget;

protected:
  osg::ref_ptr<AxisConstrainer> mAxisConstrainer;

  virtual bool handle(
    const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) {
    bool shiftDown =
      ea.getModKeyMask() & osgGA::GUIEventAdapter::MODKEY_LEFT_SHIFT;
    switch (ea.getEventType()) {
      case osgGA::GUIEventAdapter::KEYDOWN:
        switch (ea.getKey()) {
          case osgGA::GUIEventAdapter::KEY_X:
            mAxisConstrainer->addConstrain(
              shiftDown ? AxisConstrainer::CT_YZ : AxisConstrainer::CT_X);
            break;
          case osgGA::GUIEventAdapter::KEY_Y:
            mAxisConstrainer->addConstrain(
              shiftDown ? AxisConstrainer::CT_ZX : AxisConstrainer::CT_Y);
            break;
          case osgGA::GUIEventAdapter::KEY_Z:
            mAxisConstrainer->addConstrain(
              shiftDown ? AxisConstrainer::CT_XY : AxisConstrainer::CT_Z);
            break;
          default:
            break;
        }
        break;
      case osgGA::GUIEventAdapter::PUSH:
        if (ea.getButton() == osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON)
          cancel();
        else if (ea.getButton() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON)
          confirm();
        break;
      case osgGA::GUIEventAdapter::DRAG: {
        mCurrentCursor.x() = ea.getX();
        mCurrentCursor.y() = ea.getY();
        update();
        mLastCursor = mCurrentCursor;
      } break;

      default:
        break;
    }
    return false;  // return true will stop event
  }

public:
  ObjectOperation() {
    mView = camera->getViewMatrix();
    mInvView.invert(mView);

    mWnd = camera->getViewport()->computeWindowMatrix();
    mInvWnd.invert(mWnd);

    mProj = camera->getProjectionMatrix();
    mInvProj.invert(mProj);

    mProjWnd = mProj * mWnd;
    mInvProjWnd.invert(mProjWnd);

    mViewProjWnd = mView * mProjWnd;

    mModel = mTarget->getMatrix();

    osg::Vec3 p = mModel.getTrans() * mViewProjWnd;
    mStartWindowPos.x() = p.x();
    mStartWindowPos.y() = p.y();
  }

  ObjectOperation(const ObjectOperation& copy,
    const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY)
      : osg::Object(copy, copyop) {}
  ~ObjectOperation() {}
  META_Object(zxd, osg::GuiEventHandler);

  AxisConstrainer* getAxisConstrainer() const { return mAxisConstrainer; }
  void setAxisConstrainer(AxisConstrainer* v) { mAxisConstrainer = v; }

  // reset transform
  void cancel() {
    mTarget->setMatrix(mModel);
    clean();
  }
  // confirm transform
  void confirm() {
    currentOperation = 0;
    viewer->removeEventHandler(this);
    clean() :
  }
  // x y in window frame
  virtual void update(bool shiftDown = false) {
    doUpdate(shiftDown);
    updateText();
  }

  virtual void doUpdate(bool shiftDown = false) {}
  virtual void updateText() {}

  osg::Vec2 getStartCursor() const { return mStartCursor; }
  virtual void setStartCursor(osg::Vec2 v) { mStartCursor = v; }

  osg::ref_ptr<zxd::BlenderObject> getTarget() const { return mTarget; }
  void setTarget(osg::ref_ptr<zxd::BlenderObject> v) { mTarget = v; }

  // clean help stuff: like handle during scale
  virtual void clean() {
    if(mAxisConstrainer->getNumParents() != 0)
      mAxisConstrainer->getParent(0)->removeChild(mAxisConstrainer):
    osg::Node* handle = getHandle();
    if(handle->getNumParents() != 0)
      handle->getParent(0)->handle(handle):

  }
  virtual osg::Node* getHandle() { return 0; }
};

class GrabOperation : public ObjectOperation {};
class ScaleOperation : public ObjectOperation {
protected:
  float mInitalHandleLength;
  float mLastHandleLength;
  float mScale;  // used to store current operation scale
  osg::ref_ptr<zxd::DirectionArrow> mHandle;

public:
  ScaleOperation() : ObjectOperation(), mScale(1.0f) {}
  virtual void setStartCursor(osg::Vec2 v) {
    mStartCursor = v;
    mInitalHandleLength = (mStartCursor - mStartWindowPos).length();
    mLastHandleLength = mInitalHandleLength;

    // create handle
    mHandle = new DirectionArrow();
    mHandle->setMatrix(
      osg::Matrix::translate(mStartCursor.x(), mStartCursor.y(), 0));
  }
  virtual osg::Node* getHandle() { return mHandle; }

protected:
  virtual void doUpdate(bool shiftDown = false) {
    // get scale change
    GLfloat curHandleLength = (mCurrentCursor - mStartCursor).length();
    GLfloat dtScale =
      (curHandleLength - mLastHandleLength) / mInitalHandleLength;
    mScale += shiftDown ? dtScale * 0.1f : dtScale;

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
    }

    osg::Matrix scaleMatrix = osg::Matrix::scale(scaleVector);

    if (mAxisConstrainer->getConstrainFrame() == AxisConstrainer::CF_LOCAL)
      mTarget->setMatrix(scaleMatrix * mModel);
    else
      mTarget->setMatrix(mModel * scaleMatrix);
  }

  virtual void clean() {}
};
class RotateOperation : public ObjectOperation {};
}

#endif /* OBJECTOPERATION_H */
