#include <iostream>
#include <osg/Matrix>
#include "common.h"
#include <osg/io_utils>
#include <osgViewer/Viewer>

int main(int argc, char* argv[]) {
  float theta = -osg::PI_2;
  osg::Vec3 trans(0.5f, 0.5f, 0);

  osg::Matrix offset = osg::Matrix::translate(-trans);
  osg::Matrix inverseOffset = osg::Matrix::translate(trans);
  osg::Matrix rotate = osg::Matrix::rotate(-theta, osg::Z_AXIS);

  osg::Matrix texMat = offset * rotate * inverseOffset;

  osg::ref_ptr<osg::Vec4Array> vertices = new osg::Vec4Array();
  vertices->push_back(osg::Vec4(0.0f, 0.0f, 0.0f, 1.0f));
  vertices->push_back(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));
  vertices->push_back(osg::Vec4(1.0f, 1.0f, 0.0f, 1.0f));
  vertices->push_back(osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f));

  std::for_each(
    vertices->begin(), vertices->end(), [&](decltype(*vertices->begin()) v) {
      std::cout << v * texMat << std::endl;
    });

  osgViewer::Viewer viewer;
  return viewer.run();
}
