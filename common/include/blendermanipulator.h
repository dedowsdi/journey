#ifndef BLENDERMANIPULATOR_H
#define BLENDERMANIPULATOR_H

#include <osgGA/StandardManipulator>
#include <osgGA/OrbitManipulator>

namespace zxd {

/*
 * just like blender
 * drag middle button to yaw pitch
 * scroll middle button to scale
 */

class BlenderManipulator : public osgGA::OrbitManipulator {
public:
  BlenderManipulator() {}
  virtual bool performMovementLeftMouseButton(
    const double eventTimeDelta, const double dx, const double dy);
  virtual bool performMovementMiddleMouseButton(
    const double eventTimeDelta, const double dx, const double dy);
  virtual bool performMovementRightMouseButton(
    const double eventTimeDelta, const double dx, const double dy);
};
}

#endif /* BLENDERMANIPULATOR_H */
