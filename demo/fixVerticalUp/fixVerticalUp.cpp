#include <iostream>
#include <osg/Matrix>
#include "common.h"
#include <osg/io_utils>
#include <osgViewer/Viewer>
#include <osgGA/StandardManipulator>
#include <ctime>

/*
 * this function will change forward direction, what's the meanning? it's a copy
 * of StandardManipulator::fixVerticalAxis. 
 * TODO understand
 */
void fixVerticalAxis(
  osg::Quat& rotation, const osg::Vec3& localUp, bool disallowFlipOver) {
  // camera direction vectors
  osg::Vec3 cameraUp = rotation * osg::Vec3(0., 1., 0.);
  osg::Vec3 cameraRight = rotation * osg::Vec3(1., 0., 0.);
  osg::Vec3 cameraForward = rotation * osg::Vec3(0., 0., -1.);

  // computed directions
  osg::Vec3 newCameraRight1 = cameraForward ^ localUp;
  osg::Vec3 newCameraRight2 = cameraUp ^ localUp;

  OSG_NOTICE << "newCameraRight1 : " << newCameraRight1 << std::endl;
  OSG_NOTICE << "newCameraRight2 : " << newCameraRight2 << std::endl;
  osg::Vec3 newCameraRight;

  if (newCameraRight1.length2() > newCameraRight2.length2()) {
    OSG_NOTICE << "use right 1" << std::endl;
    newCameraRight = newCameraRight1;
  } else {
    OSG_NOTICE << "use right 2" << std::endl;
    newCameraRight = newCameraRight2;
  }

  if (newCameraRight * cameraRight < 0.) {
    newCameraRight = -newCameraRight;
    OSG_NOTICE << "negate camera right" << std::endl;
  }

  // vertical axis correction
  osg::Quat rotationVerticalAxisCorrection;
  rotationVerticalAxisCorrection.makeRotate(cameraRight, newCameraRight);

  // rotate camera
  rotation *= rotationVerticalAxisCorrection;

  if (disallowFlipOver) {
    // make viewer's up vector to be always less than 90 degrees from "up" axis
    osg::Vec3 newCameraUp = newCameraRight ^ cameraForward;
    if (newCameraUp * localUp < 0.)
      rotation = osg::Quat(osg::PI, osg::Vec3(0., 0., 1.)) * rotation;
  }
}

void setRow(osg::Matrix& m, osg::Vec3 v, GLuint row) {
  m(row, 0) = v[0];
  m(row, 1) = v[1];
  m(row, 2) = v[2];
}

int main(int argc, char* argv[]) {

  std::srand(std::time(0));
  osg::Vec3 eye = osg::Vec3(0, 0, 0);
  osg::Vec3 center = osg::Vec3(0, 1, 0);
  osg::Vec3 localUp = osg::Vec3(0, 0, 1);

  osg::Vec3 zAxis = eye - center;
  osg::Vec3 rightAixs = zxd::randomVector(0, 1);
  rightAixs.normalize();
  osg::Vec3 upAxis = zAxis ^ rightAixs;
  upAxis.normalize();

  osg::Matrix m;
  setRow(m, rightAixs, 0);
  setRow(m, upAxis, 1);
  setRow(m, zAxis, 2);

  OSG_NOTICE.precision(4);
  OSG_NOTICE.setf(std::ios_base::fixed);

  OSG_NOTICE << "before adjust \n" << m;

  osg::Quat q = m.getRotate();

  fixVerticalAxis(q, localUp, false);

  m.setRotate(q);

  OSG_NOTICE << "after adjust \n" << m;

  osgViewer::Viewer viewer;
  return viewer.run();
}
