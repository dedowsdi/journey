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
  protected:
    osg::Vec2 mStartCursor;
    osg::Quat mStartRotation;
    float mYaw, mPitch;

public:
  BlenderManipulator() {}
  virtual bool performMovementLeftMouseButton(
    const double eventTimeDelta, const double dx, const double dy);
  virtual bool performMovementMiddleMouseButton(
    const double eventTimeDelta, const double dx, const double dy);
  virtual bool performMovementRightMouseButton(
    const double eventTimeDelta, const double dx, const double dy);
  virtual bool handleMousePush( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us );

  virtual void rotateWithFixedVertical( const float dx, const float dy );
};
}

#endif /* BLENDERMANIPULATOR_H */
