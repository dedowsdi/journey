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
  bool mOrtho; //only 9 works in ortho view
  osg::Vec2 mStartCursor;
  osg::Quat mStartRotation;
  float mYaw, mPitch;
  float mRotateStep;  // yaw, pitch step of numpad
  float mPanStep;     // yaw, pitch step of numpad
  osg::ref_ptr<osgText::Text> mViewText;

  // becareful fovy not in radian
  double mPerspFovy;

  osg::Camera* mCamera;

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

  void viewLeft(const osgGA::GUIEventAdapter& ea) {
    animRotView(osg::PI_2, -osg::PI_2, ea.getTime());
  }
  void viewRight(const osgGA::GUIEventAdapter& ea) {
    animRotView(osg::PI_2, osg::PI_2, ea.getTime());
  }
  void viewFront(const osgGA::GUIEventAdapter& ea) {
    animRotView(osg::PI_2, 0, ea.getTime());
  }
  void viewBack(const osgGA::GUIEventAdapter& ea) {
    animRotView(osg::PI_2, osg::PI, ea.getTime());
  }
  void viewTop(const osgGA::GUIEventAdapter& ea) {
    animRotView(0, 0, ea.getTime());
  }
  void viewBottom(const osgGA::GUIEventAdapter& ea) {
    animRotView(osg::PI, 0, ea.getTime());
  }
  // toggle left right, back front, bottom top
  void viewInverse(const osgGA::GUIEventAdapter& ea);

  void animRotView(float destPitch, float destYaw, float time);

  void animPanZoom(float destDistance, const osg::Vec3& destCenter, float time);

  void yaw(GLfloat v);
  void pitch(GLfloat v);

  void toggleProjectionType();

  osg::ref_ptr<osgText::Text> getViewText() const { return mViewText; }
  void setViewText(osg::ref_ptr<osgText::Text> v) { mViewText = v; }

  osg::Camera* getCamera() const { return mCamera; }
  void setCamera(osg::Camera* v) { mCamera = v; }

  bool getOrtho() const { return mOrtho; }
  void setOrtho( bool v){mOrtho = v;}

  bool isOrthoType();

  float getDistance(){return _distance;}

  void setRotation(GLfloat pitch, float yaw);

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
