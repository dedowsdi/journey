#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osg/MatrixTransform>
#include <osg/io_utils>
#include <iostream>

/*
 * Gimbal machine works just like intrinsic euler rotation. It has three
 * circular arms to mimic x y z intrinsic rotation.
 *
 * In XYZ intrinsic euler rotation.
 * if Y rotates PI_2, Z will coinsides with initial x, in this case, X and Z
 * has the same effect( the outside and inside circular arm will resides in the
 * same plane). This will lock one rotation dimension, and the gimbal
 * machine will be locked.
 */


void createEulerXYZ(GLfloat x, GLfloat y, GLfloat z){
  osg::Matrix m;
  m.makeRotate(z, osg::Z_AXIS, y, osg::Y_AXIS, x, osg::X_AXIS );
  std::cout << m << std::endl;
}



int main(int argc, char* argv[]) {

  //as long as x+z is the same, the output should be the same
  createEulerXYZ(osg::PI_4, osg::PI_2, osg::PI_4);
  createEulerXYZ(0, osg::PI_2, osg::PI_2);
  createEulerXYZ(osg::PI_2, osg::PI_2, 0);

  osgViewer::Viewer viewer;
  viewer.addEventHandler(new osgViewer::StatsHandler);
  //viewer.setSceneData();

  return viewer.run();
}
