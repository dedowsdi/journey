#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osg/Geometry>
#include <osgDB/ReadFile>
#include <osg/MatrixTransform>
#include <osgUtil/Tessellator>
#include <osgUtil/SmoothingVisitor>

/*
 * revolve curve along axis, only create side contours. close if degree * count
 * = 2 pi
 */
osg::ref_ptr<osg::Geometry> revolution(const osg::ref_ptr<osg::Vec3Array> curve,
  GLfloat degree, const osg::Vec3& axis, GLuint  count) {
  // copy curve
  osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array(*curve);

  bool close = std::abs(degree * count - osg::PI*2) <= 0.001;
  GLuint curveSize = curve->size();

  // revolve
  for (GLuint  i = 1; i <= count; ++i) {
    GLfloat d = i * degree;
    if (i != count || !close) {
      osg::Quat quat(d, axis);
      for (GLuint  j = 0; j < curveSize; ++j) {
        vertices->push_back(quat * curve->at(j));
      }
    }
  }

  // create geometry
  osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry();
  geometry->setVertexArray(vertices.get());

  // create quad strips circle by circle
  for (GLuint row = 0; row < curveSize - 1; ++row) {
    osg::ref_ptr<osg::DrawElementsUInt> elements = new osg::DrawElementsUInt(GL_QUAD_STRIP);
    elements->reserve(2 * (count + 1));
    for (GLuint col = 0; col < count; ++col) {
      elements->push_back(curveSize * col + row + 1);
      elements->push_back(curveSize * col + row);
      if (close && col == count - 1) {
        // close
        elements->push_back(row + 1);
        elements->push_back(row);
      } else {
        elements->push_back(curveSize * (col + 1) + row + 1);
        elements->push_back(curveSize * (col + 1) + row);
      }
    }
    geometry->addPrimitiveSet(elements.get());
  }

  // generate normal
  osgUtil::SmoothingVisitor::smooth(*geometry);

  return geometry;
}

int main(int argc, char* argv[]) {
  // read axis and degree
  osg::ArgumentParser ap(&argc, argv);
  osg::Vec3 axis(0, 0, 1);
  GLfloat degree = 30;
  GLfloat count = 12;
  ap.read("--axis", axis.x(), axis.y(), axis.z());
  ap.read("--degree", degree);
  ap.read("--count", count);
  degree = osg::DegreesToRadians(degree);

  // create a curve
  osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array(4);
  vertices->at(0).set(1, 0, 0);
  vertices->at(1).set(1.5, 0, 0.5);
  vertices->at(2).set(2, 0, 1);
  vertices->at(3).set(3, 0, 1.5);

  osg::ref_ptr<osg::Geometry> geometry =
    revolution(vertices, degree, axis, count);
  osg::ref_ptr<osg::Geode> node = new osg::Geode();
  node->addDrawable(geometry.get());

  osgViewer::Viewer viewer;
  viewer.addEventHandler(new osgViewer::StatsHandler);
  viewer.setSceneData(node);

  return viewer.run();
}
