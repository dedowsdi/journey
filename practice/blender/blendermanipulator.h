#ifndef BLENDERMANIPULATOR_H
#define BLENDERMANIPULATOR_H

#include <osgGA/StandardManipulator>
#include <osgGA/OrbitManipulator>
#include <osgAnimation/EaseMotion>
#include <osgText/Text>

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
  osg::Vec2 mStartCursor;
  osg::Quat mStartRotation;
  float mYaw, mPitch;
  float mRotateStep;  // yaw, pitch step of numpad
  float mPanStep;     // yaw, pitch step of numpad
  Blender* mBlender;
  osg::ref_ptr<osgText::Text> mViewText;

  // becareful fovy not in radian
  double mPerspFovy, mPerspAspectRatio, mPerspZNear, mPerspZFar;
  double mOrthoLeft, mOrthoRight, mOrthoTop, mOrthoBottom, mOrthoNear,
    mOrthoFar;

public:
  BlenderManipulator()
      : mYaw(0.0f),
        mPitch(0.0f),
        mRotateStep(osg::PI_2 / 6.0f),
        mPanStep(0.06f) {
    setAnimationTime(0.2f);
    setAllowThrow(false);
  }
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

  inline void animRotView(float destPitch, float destYaw, float time) {
    OSG_NOTICE << "start animation. destPitch : " << destPitch
               << ", destYaw : " << destYaw << std::endl;
    auto ad = getBlenderAnimData();
    ad->init(this);
    ad->mDestPitch = destPitch;
    ad->mDestYaw = destYaw;
    ad->start(time);
  }

  inline void animPanZoom(
    float destDistance, const osg::Vec3& destCenter, float time) {
    auto ad = getBlenderAnimData();
    ad->init(this);
    ad->mDestDistance = destDistance;
    ad->mDestCenter = destCenter;
    ad->start(time);
  }

  void yaw(GLfloat v);
  void pitch(GLfloat v);

  void toggleProjectionType();

  Blender* getBlender() const { return mBlender; }
  void setBlender(Blender* v);

  osg::ref_ptr<osgText::Text> getViewText() const { return mViewText; }
  void setViewText(osg::ref_ptr<osgText::Text> v) { mViewText = v; }

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
  void clampAngle() {
    static float twoPi = osg::PI * 2;
    mYaw = std::fmod(mYaw, twoPi);
    mPitch = std::fmod(mPitch, twoPi);
    if (mYaw < 0) mYaw += twoPi;
    if (mPitch < 0) mPitch += twoPi;
  }

  // virtual void rotateWithFixedVertical( const float dx, const float dy );
};
}

#endif /* BLENDERMANIPULATOR_H */
