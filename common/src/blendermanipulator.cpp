
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
  osgGA::OrbitManipulator::performMovementLeftMouseButton(
    eventTimeDelta, dx, dy);
  return false;
}

//------------------------------------------------------------------------------
bool BlenderManipulator::performMovementRightMouseButton(
  const double eventTimeDelta, const double dx, const double dy) {
  return false;
}
}
