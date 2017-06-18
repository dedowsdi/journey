#ifndef OBJECTOPERATION_H
#define OBJECTOPERATION_H

#include <osgGA/GUIEventHandler>
#include "axisconstrainer.h"
#include "auxiliary.h"
#include "common.h"
#include <osg/LineStipple>
#include <osg/io_utils>

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

  osg::Vec2 mWndPivot;
  osg::Vec2 mLastCursor;
  osg::Vec2 mCurrentCursor;

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

public:
  // ObjectOperation() {}

  // ObjectOperation(const ObjectOperation& copy,
  // const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY)
  //: osg::GUIEventHandler(copy, copyop) {}
  //~ObjectOperation() {}
  // META_Object(zxd, ObjectOperation);

  AxisConstrainer* getAxisConstrainer() const { return mAxisConstrainer; }
  void setAxisConstrainer(AxisConstrainer* v) { mAxisConstrainer = v; }

  // reset transform
  void cancel() {
    mTarget->setMatrix(mModel);
    std::cout << mModel << std::endl;
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
    mWndPivot.x() = p.x();
    mWndPivot.y() = p.y();
    mCurrentCursor = v;

    doInit();
  }
  virtual void doInit() {}

  osg::ref_ptr<zxd::BlenderObject> getTarget() const { return mTarget; }
  void setTarget(osg::ref_ptr<zxd::BlenderObject> v) { mTarget = v; }

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
    v0 = osg::Vec3(osg::Vec3(mWndPivot.x(), mWndPivot.y(), 0));
    v1 = osg::Vec3(osg::Vec3(mCurrentCursor.x(), mCurrentCursor.y(), 0));
  }
};

class GrabOperation : public ObjectOperation {};
class ScaleOperation : public ObjectOperation {
protected:
  float mInitalHandleLength;
  float mLastHandleLength;
  float mScale;  // used to store current operation scale
  osg::ref_ptr<osg::Group> mHandle;

public:
  ScaleOperation() : ObjectOperation(), mScale(1.0f) {}
  virtual void doInit() {
    mInitalHandleLength = (mCurrentCursor - mWndPivot).length();
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
    da->setDirection(0, mCurrentCursor - mWndPivot);
    da->setDirection(1, mWndPivot - mCurrentCursor);

    mHandle->addChild(lsn);
    mHandle->addChild(da);

    OSG_NOTICE << "create scale handle" << std::endl;
  }
  virtual osg::Node* getHandle() { return mHandle; }

protected:
  virtual void doUpdate(bool shiftDown) {
    // get scale change
    GLfloat curHandleLength = (mCurrentCursor - mWndPivot).length();
    GLfloat dtScale =
      (curHandleLength - mLastHandleLength) / mInitalHandleLength;
    mLastHandleLength = curHandleLength;
    
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
      default:
        throw std::runtime_error("unknown constrain type");
    }

    osg::Matrix scaleMatrix = osg::Matrix::scale(scaleVector);

    if (mAxisConstrainer->getConstrainFrame() == AxisConstrainer::CF_LOCAL)
      mTarget->setMatrix(scaleMatrix * mModel);
    else
      mTarget->setMatrix(mModel * scaleMatrix);

    // change handle and arrow direction
    zxd::LineSegmentNode* lsn =
      static_cast<zxd::LineSegmentNode*>(mHandle->getChild(0));
    zxd::DirectionArrow* da =
      static_cast<zxd::DirectionArrow*>(mHandle->getChild(1));

    osg::Vec3 v0, v1;
    getHandlePosition(v0, v1);

    lsn->setEndPosition(v1);
    //OSG_NOTICE << "set handle end position : " << v1 << std::endl;
    
    da->setMatrix(osg::Matrix::translate(v1));
    da->setDirection(0, mCurrentCursor - mWndPivot);
    da->setDirection(1, mWndPivot - mCurrentCursor);
  }

};
class RotateOperation : public ObjectOperation {};
}

#endif /* OBJECTOPERATION_H */
