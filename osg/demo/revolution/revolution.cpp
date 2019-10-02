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
osg::ref_ptr<osg::Geometry> revolution(const osg::Vec3Array& curve,
  GLfloat degree, const osg::Vec3& axis, GLuint  count) {
  // copy curve
  osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array(curve);

  bool close = std::abs(degree * count - osg::PI*2) <= 0.001;
  GLuint curveSize = curve.size();

  // revolve
  for (GLuint i = 1; i < count; ++i)
  {
    GLfloat d = i * degree;
    osg::Quat quat(d, axis);
    for (GLuint j = 0; j < curveSize; ++j)
      vertices->push_back(quat * curve[j]);
  }

  if (close)
    vertices->insert(vertices->end(), curve.begin(), curve.end());

  // create geometry
  osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry();
  geometry->setVertexArray(vertices.get());

  // create quad strips circle by circle
  for (GLuint col = 0; col < count; ++col)
  {
    auto elements = new osg::DrawElementsUInt(GL_QUAD_STRIP);
    elements->reserve(2 * (count + 1));
    auto start0 = col * curveSize + 1;
    auto start1 = start0 + curveSize;
    for (GLuint row = 0; row < curveSize - 1; ++row)
    {
      elements->push_back(start0 + row);
      elements->push_back(start1 + row);
    }
    geometry->addPrimitiveSet(elements);
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
    revolution(*vertices, degree, axis, count);
  osg::ref_ptr<osg::Geode> node = new osg::Geode();
  node->addDrawable(geometry.get());

  osgViewer::Viewer viewer;
  viewer.addEventHandler(new osgViewer::StatsHandler);
  viewer.setSceneData(node);

  return viewer.run();
}
