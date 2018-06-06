#ifndef BLENDERMANIPULATOR_H
#define BLENDERMANIPULATOR_H

#include <osgGA/StandardManipulator>
#include <osgGA/OrbitManipulator>
#include <osgAnimation/EaseMotion>
#include <osgText/Text>
#include "blenderview.h"

namespace zxd {

/*
 * just like blender
 * drag middle button to yaw pitch
 * scroll middle button to scale
 *
 * pitch along view x, yaw along world y
 */

class Blender;

class BlenderManipulator : public osgGA::OrbitManipulator {
protected:
  bool mLock; // only setRotation, setCenter, setDistance works if mLock is true
  osg::Quat mStartRotation;
  float mYaw, mPitch; // intrinsic pitch, yaw in view space
  float mRotateStep;  // yaw, pitch step of numpad 2 4 6 8
  float mPanStep;     // pan step of arrow
  osg::ref_ptr<osgText::Text> mViewText;

  double mPerspFovy;

  osg::Camera* mCamera;
  osg::Timer_t mStartTick;

public:
  BlenderManipulator();
  virtual bool handleKeyDown(
    const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us);

  virtual bool performMovementLeftMouseButton(
    const double eventTimeDelta, const double dx, const double dy) {
    return false;
  }
  virtual bool performMovementMiddleMouseButton(
    const double eventTimeDelta, const double dx, const double dy);
  virtual bool performMovementRightMouseButton(
    const double eventTimeDelta, const double dx, const double dy) {
    return false;
  }

  virtual void setByMatrix(const osg::Matrixd& matrix);
  virtual void setTransformation(
    const osg::Vec3d& eye, const osg::Quat& rotation);
  virtual void setTransformation(
    const osg::Vec3d& eye, const osg::Vec3d& center, const osg::Vec3d& up);

  virtual void applyAnimationStep(
    const double currentProgress, const double prevProgress);

  float getPitch() const { return mPitch; }
  float getYaw() const { return mYaw; }

  void updateRotation();
  void updateText();


  void left(bool anim = false);
  void right(bool anim = false);
  void front(bool anim = false);
  void back(bool anim = false);
  void top(bool anim = false);
  void bottom(bool anim = false);
  void inverse(bool anim = false);

  void animRotView(float destPitch, float destYaw);

  void animPanZoom(float destDistance, const osg::Vec3& destCenter);

  void yaw(GLfloat v);
  void pitch(GLfloat v);

  void toggleProjectionType();

  osg::ref_ptr<osgText::Text> getViewText() const {return mViewText; }
  void setViewText(osg::ref_ptr<osgText::Text> v) { mViewText = v; }

  osg::Camera* getCamera() const { return mCamera; }
  void setCamera(osg::Camera* v) { mCamera = v; }

  bool getLock() const { return mLock; }
  void setLock( bool v){mLock = v;}

  bool isOrthoType();

  float getDistance(){return _distance;}

  void setRotation(GLfloat pitch, float yaw);

  osg::Timer_t getStartTick() const { return mStartTick; }
  void setStartTick(osg::Timer_t v){ mStartTick = v; }

protected:
  class BlenderAnimationData
    : public ::osgGA::StandardManipulator::AnimationData {
  public:
    void init(zxd::BlenderManipulator* om);

    float mSrcYaw;
    float mSrcPitch;
    float mDestYaw;
    float mDestPitch;
    float mSrcDistance;
    float mDestDistance;
    osg::Vec3 mSrcCenter;
    osg::Vec3 mDestCenter;

    osg::ref_ptr<osgAnimation::InOutCubicMotion> mMotion;
  };

  virtual void allocAnimationData() {
    _animationData = new BlenderAnimationData();
  }
  inline zxd::BlenderManipulator::BlenderAnimationData* getBlenderAnimData() {
    return osg::static_pointer_cast<
      zxd::BlenderManipulator::BlenderAnimationData>(_animationData);
  }

  // clamp yaw pitch to 0-2pi
  void clampAngle();

  // virtual void rotateWithFixedVertical( const float dx, const float dy );
};
}

#endif /* BLENDERMANIPULATOR_H */
