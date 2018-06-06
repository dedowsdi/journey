/*
 * if your contour composed of simple quads, you can set draw mode to
 * GL_LINE_LOOP to draw border lines
 */
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osg/Geometry>
#include <osgDB/ReadFile>
#include <osg/MatrixTransform>
#include <osg/LineWidth>
#include <osgUtil/Tessellator>

int main(int argc, char* argv[]) {

  osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array(8);
  (*vertices)[0].set(0.0f, 0.0f, 0.0f);
  (*vertices)[1].set(3.0f, 0.0f, 0.0f);
  (*vertices)[2].set(3.0f, 0.0f, 3.0f);
  (*vertices)[3].set(0.0f, 0.0f, 3.0f);
  (*vertices)[4].set(1.0f, 0.0f, 1.0f);
  (*vertices)[5].set(2.0f, 0.0f, 1.0f);
  (*vertices)[6].set(2.0f, 0.0f, 2.0f);
  (*vertices)[7].set(1.0f, 0.0f, 2.0f);

  osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array(1);
  (*normals)[0].set(0.0f, -1.0f, 0.0f);

  osg::ref_ptr<osg::Geometry> polygon = new osg::Geometry;
  polygon->setVertexArray(vertices.get());
  polygon->setNormalArray(normals.get());
  polygon->setNormalBinding(osg::Geometry::BIND_OVERALL);
  polygon->addPrimitiveSet(new osg::DrawArrays(GL_QUADS, 0, 4));
  polygon->addPrimitiveSet(new osg::DrawArrays(GL_QUADS, 4, 4));

  //tessellate to conver concave contour to convex contour
  osgUtil::Tessellator tessellator;
  tessellator.setTessellationType(osgUtil::Tessellator::TESS_TYPE_GEOMETRY);
  tessellator.setWindingType(osgUtil::Tessellator::TESS_WINDING_ODD);
  tessellator.retessellatePolygons(*polygon);

  osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array(1);
  (*colors)[0].set(1.0f, 1.0f, 0.0f, 1.0f);

  //use the same vertices to draw border line
  osg::ref_ptr<osg::Geometry> border = new osg::Geometry;
  border->setVertexArray(vertices.get());
  border->setColorArray(colors.get());
  border->setColorBinding(osg::Geometry::BIND_OVERALL);
  border->addPrimitiveSet(new osg::DrawArrays(GL_LINE_LOOP, 0, 4));
  border->addPrimitiveSet(new osg::DrawArrays(GL_LINE_LOOP, 4, 4));
  border->getOrCreateStateSet()->setAttribute(new osg::LineWidth(5.0f));

  osg::ref_ptr<osg::Geode> geode = new osg::Geode;
  geode->addDrawable(polygon.get());
  geode->addDrawable(border.get());
  osg::ref_ptr<osg::Group> root = new osg::Group;
  root->addChild(geode.get());

  osgViewer::Viewer viewer;
  viewer.setSceneData(root.get());
  return viewer.run();
}
