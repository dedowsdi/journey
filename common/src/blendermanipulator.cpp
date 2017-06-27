
#include "blendermanipulator.h"

namespace zxd {

//------------------------------------------------------------------------------
bool BlenderManipulator::performMovementLeftMouseButton(
  const double eventTimeDelta, const double dx, const double dy) {
  return false;
}

//------------------------------------------------------------------------------
bool BlenderManipulator::performMovementMiddleMouseButton(
  const double eventTimeDelta, const double dx, const double dy) {
  osg::Vec2 offset(
    _ga_t0->getX() - _ga_t1->getX(), _ga_t0->getY() - _ga_t1->getY());
  offset *= 0.01f;
  mYaw -= offset.x();
  mPitch += offset.y();

  _rotation = osg::Quat(mPitch, osg::X_AXIS) * osg::Quat(mYaw, osg::Y_AXIS) *
              osg::Quat(osg::PI_2, osg::X_AXIS);

  return false;
}

//------------------------------------------------------------------------------
bool BlenderManipulator::performMovementRightMouseButton(
  const double eventTimeDelta, const double dx, const double dy) {
  return false;
}

//------------------------------------------------------------------------------
bool BlenderManipulator::handleMousePush(
  const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us) {
  mStartCursor = osg::Vec2(ea.getX(), ea.getY());
  mStartRotation = _rotation;
  return StandardManipulator::handleMousePush(ea, us);
}

//------------------------------------------------------------------------------
void BlenderManipulator::rotateWithFixedVertical(
  const float dx, const float dy) {}
}
