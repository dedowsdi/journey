#include "blendermanipulator.h"
#include "zmath.h"
#include <cassert>
#include "blenderobject.h"
#include "blender.h"
#include <osg/ComputeBoundsVisitor>
#include "prerequisite.h"

namespace zxd {

//------------------------------------------------------------------------------
BlenderManipulator::BlenderManipulator()
    : mYaw(0.0f),
      mPitch(0.0f),
      mRotateStep(osg::PI_2 / 6.0f),
      mPanStep(0.06f),
      mCamera(0),
      mLock(false) {
  setAnimationTime(0.2f);
  setAllowThrow(false);

  osg::DisplaySettings* ds = osg::DisplaySettings::instance();
  mPerspFovy = osg::RadiansToDegrees(atan2(ds->getScreenHeight() / 2.0f, ds->getScreenDistance()) * 2.0);
}

//------------------------------------------------------------------------------
void BlenderManipulator::BlenderAnimationData::init(
  zxd::BlenderManipulator* om) {
  if (!mMotion) mMotion = new osgAnimation::InOutCubicMotion();
  mMotion->reset();
  mSrcYaw = mDestYaw = om->mYaw;
  mSrcPitch = mDestPitch = om->mPitch;
  mSrcDistance = mDestDistance = om->_distance;
  mSrcCenter = mDestCenter = om->_center;
}

//------------------------------------------------------------------------------
void BlenderManipulator::clampAngle() {
  mYaw = zxd::Math::clampAngle(mYaw);
  mPitch = zxd::Math::clampAngle(mPitch);
}

//------------------------------------------------------------------------------
bool BlenderManipulator::handleKeyDown(
  const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us) {
  switch (ea.getUnmodifiedKey()) {
    case osgGA::GUIEventAdapter::KEY_KP_End: {
      if (ea.getModKeyMask() & osgGA::GUIEventAdapter::MODKEY_CTRL)
        back(true);
      else
        front(true);
    } break;

    case osgGA::GUIEventAdapter::KEY_KP_Page_Down: {
      if (ea.getModKeyMask() & osgGA::GUIEventAdapter::MODKEY_CTRL)
        right(true);
      else
        left(true);
    } break;

    case osgGA::GUIEventAdapter::KEY_KP_Home: {
      if (ea.getModKeyMask() & osgGA::GUIEventAdapter::MODKEY_CTRL)
        bottom(true);
      else
        top(true);
    } break;

    case osgGA::GUIEventAdapter::KEY_KP_Page_Up: {
      inverse(true);
    } break;

    case osgGA::GUIEventAdapter::KEY_KP_Left: {
      if (ea.getModKeyMask() & osgGA::GUIEventAdapter::MODKEY_SHIFT)
        panModel(-mPanStep * -0.3f * _distance, 0);
      else
        yaw(-mRotateStep);
    } break;
    case osgGA::GUIEventAdapter::KEY_KP_Right: {
      if (ea.getModKeyMask() & osgGA::GUIEventAdapter::MODKEY_SHIFT)
        panModel(mPanStep * -0.3f * _distance, 0);
      else
        yaw(mRotateStep);
    } break;

    case osgGA::GUIEventAdapter::KEY_KP_Up: {
      if (ea.getModKeyMask() & osgGA::GUIEventAdapter::MODKEY_SHIFT)
        panModel(0, mPanStep * -0.3f * _distance);
      else
        pitch(-mRotateStep);
    } break;
    case osgGA::GUIEventAdapter::KEY_KP_Down: {
      if (ea.getModKeyMask() & osgGA::GUIEventAdapter::MODKEY_SHIFT)
        panModel(0, -mPanStep * -0.3f * _distance);
      else
        pitch(mRotateStep);
    } break;

    case osgGA::GUIEventAdapter::KEY_Home:
      home(ea, us);
      break;

    case osgGA::GUIEventAdapter::KEY_KP_Delete: {
      // zoom to selection
      zxd::BlenderObject* bo = sgBlender->getCurObject();
      if (bo) {
         osg::ComputeBoundsVisitor cbVisitor;
         bo->accept(cbVisitor);
         osg::BoundingSphere bs;
         bs.expandRadiusBy(cbVisitor.getBoundingBox());

         //what's the difference between this and the codes above ?
        //const osg::BoundingSphere& bs = bo->getBound();

        float fovy, aspectRatio, zNear, zFar;
        mCamera->getProjectionMatrix().getPerspective(
          fovy, aspectRatio, zNear, zFar);

        float destDistance =
          bs.radius() / sinf(osg::DegreesToRadians(fovy) * 0.5f);
        animPanZoom(destDistance, bo->getMatrix().getTrans());
      }
    } break;

    case osgGA::GUIEventAdapter::KEY_KP_Begin:  // this is 5 in key pad
      toggleProjectionType();
      break;

    default:
      break;
  }
  return false;
}

//------------------------------------------------------------------------------
bool BlenderManipulator::performMovementMiddleMouseButton(
  const double eventTimeDelta, const double dx, const double dy) {

  if(getLock())
    return true;

  // check shift status
  if (_ga_t0->getModKeyMask() & osgGA::GUIEventAdapter::MODKEY_SHIFT) {
    // pan model
    float scale = -0.3f * _distance;
    panModel(dx * scale, dy * scale);
  } else {
    // rotate
    osg::Vec2 offset(
      _ga_t0->getX() - _ga_t1->getX(), _ga_t0->getY() - _ga_t1->getY());
    offset *= 0.01f;
    mYaw += offset.x();
    mPitch -= offset.y();
    updateRotation();
  }

  return true;
}

//------------------------------------------------------------------------------
void BlenderManipulator::setByMatrix(const osg::Matrixd& matrix) {
  _center = osg::Vec3d(0., 0., -_distance) * matrix;

  // get yaw and pitch
  osg::Vec3 xAxis(matrix(0, 0), matrix(0, 1), matrix(0, 2));
  osg::Vec3 yAxis(matrix(1, 0), matrix(1, 1), matrix(1, 2));
  osg::Vec3 zAxis(matrix(2, 0), matrix(2, 1), matrix(2, 2));

  // see updateRotation for math explanation
  mYaw = -atan2(xAxis[1], xAxis[0]);
  mPitch = -zxd::Math::angleSigned(osg::Z_AXIS, zAxis, xAxis);
  updateRotation();
}

//------------------------------------------------------------------------------
void BlenderManipulator::setTransformation(
  const osg::Vec3d& eye, const osg::Quat& rotation) {
  setByMatrix(osg::Matrix::rotate(rotation) * osg::Matrix::translate(eye));
}

//------------------------------------------------------------------------------
void BlenderManipulator::setTransformation(
  const osg::Vec3d& eye, const osg::Vec3d& center, const osg::Vec3d& up) {
  _distance = (eye - center).length();
  osg::Matrix view = osg::Matrix::lookAt(eye, center, up);
  setByMatrix(osg::Matrix::inverse(view));
}

//------------------------------------------------------------------------------
void BlenderManipulator::applyAnimationStep(
  const double currentProgress, const double prevProgress) {
  auto ad = getBlenderAnimData();
  assert(ad);
  ad->mMotion->setTime(currentProgress);
  float f = ad->mMotion->getValue();

  mYaw = zxd::Math::linearInterpolate(ad->mSrcYaw, ad->mDestYaw, f);
  mPitch = zxd::Math::linearInterpolate(ad->mSrcPitch, ad->mDestPitch, f);
  _distance =
    zxd::Math::linearInterpolate(ad->mSrcDistance, ad->mDestDistance, f);
  _center = zxd::Math::linearInterpolate(ad->mSrcCenter, ad->mDestCenter, f);

  clampAngle();
  updateRotation();
}

//------------------------------------------------------------------------------
void BlenderManipulator::updateRotation() {
  /*
   * in column major matrix system.
   *
   * pitch scene along view x(local x), yaw along wolrd z
   *
   * so q0 = p0 * I * y0
   *    q1 = p1 * q0 * y1
   *       = p0 * p1 * q0 * y0 * y1
   *  .......
   *    q = p * y
   *    q_i = y_i * p_i
   *
   *  so pitch and yaw an be accumulated
   */
  _rotation = osg::Quat(-mPitch, osg::X_AXIS) * osg::Quat(-mYaw, osg::Z_AXIS);
  updateText();
}

//------------------------------------------------------------------------------
void BlenderManipulator::updateText() {
  if (!mCamera) return;

  // update text
  std::string projType, rotType("User");

  double left, right, top, bottom, near, far;
  bool isPerspective =
    mCamera->getProjectionMatrixAsFrustum(left, right, top, bottom, near, far);
  projType = isPerspective ? "Persp" : "Ortho";

  if (zxd::Math::isAboutf(mPitch, osg::PI_2)) {
    if (zxd::Math::isAboutf(mYaw, 0))
      rotType = "Front";
    else if (zxd::Math::isAboutf(mYaw, osg::PI_2))
      rotType = "Right";
    else if (zxd::Math::isAboutf(mYaw, osg::PI))
      rotType = "Back";
    else if (zxd::Math::isAboutf(mYaw, osg::PI_2 * 3))
      rotType = "Left";
  } else if (zxd::Math::isAboutf(mYaw, 0.0f)) {
    if (zxd::Math::isAboutf(mPitch, 0))
      rotType = "Up";
    else if (zxd::Math::isAboutf(mPitch, osg::PI))
      rotType = "bottom";
  }

  mViewText->setText(rotType + " " + projType);
}

//--------------------------------------------------------------------
void BlenderManipulator::left(bool anim/* = false*/) 
{
  if(!anim)
    setRotation(-osg::PI_2, osg::PI_2); 
  else
    animRotView(-osg::PI_2, osg::PI_2); 
}

//--------------------------------------------------------------------
void BlenderManipulator::right(bool anim/* = false*/) 
{
  if(!anim)
    setRotation(-osg::PI_2, -osg::PI_2); 
  else
    animRotView(-osg::PI_2, -osg::PI_2); 
}

//--------------------------------------------------------------------
void BlenderManipulator::front(bool anim/* = false*/) 
{
  if(!anim)
    setRotation(-osg::PI_2, 0); 
  else
    animRotView(-osg::PI_2, 0); 
}

//--------------------------------------------------------------------
void BlenderManipulator::back(bool anim/* = false*/) 
{
  if(!anim)
    setRotation(-osg::PI_2, -osg::PI); 
  else
    animRotView(-osg::PI_2, -osg::PI); 
}

//--------------------------------------------------------------------
void BlenderManipulator::top(bool anim/* = false*/) 
{
  if(!anim)
    setRotation(0, 0); 
  else
    animRotView(0, 0); 
}

//--------------------------------------------------------------------
void BlenderManipulator::bottom(bool anim/* = false*/) 
{
  if(!anim)
    setRotation(-osg::PI, 0); 
  else
    animRotView(-osg::PI, 0); 
}

//------------------------------------------------------------------------------
void BlenderManipulator::inverse(bool anim/* = false*/) {
  static float epsilon = 0.001f;

  auto ad = getBlenderAnimData();
  float destPitch, destYaw;

  // get current pitch including animation
  float curDestPitch = ad->_isAnimating ? ad->mDestPitch : mPitch;
  float curDestYaw = ad->_isAnimating ? ad->mDestYaw : mYaw;

  float absmod = std::abs(std::fmod(curDestPitch, osg::PI));

  if (absmod <= epsilon || osg::PI - absmod <= epsilon) {
    // pitch 180 only if it's top or bottom view
    destPitch = curDestPitch + osg::PI;
    destYaw = curDestYaw;
  } else {
    destPitch = curDestPitch;
    destYaw = curDestYaw + osg::PI;
  }

  // clamp angle, avoid circle rotation
  destPitch = zxd::Math::clampAngle(destPitch);
  destYaw = zxd::Math::clampAngle(destYaw);

  if (!anim) {
    // no animation
    setRotation(destPitch, destYaw);

  } else {
    OSG_INFO << "start animation. destPitch : " << destPitch
             << ", destYaw : " << destYaw << std::endl;
    ad->init(this);
    ad->mDestPitch = destPitch;
    ad->mDestYaw = destYaw;
    ad->start(_ga_t0->getTime());
  }
}

//------------------------------------------------------------------------------
void BlenderManipulator::animRotView( float destPitch, float destYaw) {
  if (getLock()) return;

  // clamp angle, avoid circle rotation
  destPitch = zxd::Math::clampAngle(destPitch);
  destYaw = zxd::Math::clampAngle(destYaw);
  OSG_INFO << "start animation. destPitch : " << destPitch
           << ", destYaw : " << destYaw << std::endl;
  auto ad = getBlenderAnimData();
  ad->init(this);
  ad->mDestPitch = destPitch;
  ad->mDestYaw = destYaw;
  ad->start(_ga_t0->getTime());
}

//------------------------------------------------------------------------------
inline void BlenderManipulator::animPanZoom(
  float destDistance, const osg::Vec3& destCenter) {
  if (getLock()) return;

  auto ad = getBlenderAnimData();
  ad->init(this);
  ad->mDestDistance = destDistance;
  ad->mDestCenter = destCenter;
  ad->start(_ga_t0->getTime());
}

//------------------------------------------------------------------------------
void BlenderManipulator::yaw(GLfloat v) {
  if (getLock()) return;

  mYaw += v;
  clampAngle();
  updateRotation();
}

//------------------------------------------------------------------------------
void BlenderManipulator::pitch(GLfloat v) {
  if (getLock()) return;

  mPitch += v;
  clampAngle();
  updateRotation();
}

//------------------------------------------------------------------------------
void BlenderManipulator::toggleProjectionType() {
  // becareful fovy not in radian

  const osg::Matrix& proj = mCamera->getProjectionMatrix();
  osg::Matrix nextProj;
  if (zxd::Math::perspToOrtho(proj, _distance, nextProj) ||
      zxd::Math::orthoToPersp(proj, mPerspFovy, nextProj))
    mCamera->setProjectionMatrix(nextProj);

  updateText();
}

//------------------------------------------------------------------------------
void BlenderManipulator::setRotation(GLfloat pitch, float yaw) {
  mPitch = pitch;
  mYaw = yaw;
  clampAngle();
  updateRotation();
}
}
