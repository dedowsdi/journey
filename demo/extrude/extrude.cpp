#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osg/Geometry>
#include <osgDB/ReadFile>
#include <osg/MatrixTransform>
#include <osgUtil/Tessellator>
#include <osgUtil/SmoothingVisitor>

osg::Geometry* createExtrusion(
  osg::Vec3Array* vertices, const osg::Vec3& direction, float length) {

  //copy bottom vertices
  osg::ref_ptr<osg::Vec3Array> newVertices = new osg::Vec3Array(*vertices);

  //create top vertices in reverse order.
  //TODO figure out y in reverse order
  unsigned int numVertices = vertices->size();
  osg::Vec3 offset = direction * length;
  for (osg::Vec3Array::reverse_iterator ritr = vertices->rbegin();
       ritr != vertices->rend(); ++ritr) {
    newVertices->push_back((*ritr) + offset);
  }

  //create geometry
  osg::ref_ptr<osg::Geometry> extrusion = new osg::Geometry;
  extrusion->setVertexArray(newVertices.get());

  //add bottom and to prpimitive set
  extrusion->addPrimitiveSet(new osg::DrawArrays(GL_POLYGON, 0, numVertices));
  extrusion->addPrimitiveSet(
    new osg::DrawArrays(GL_POLYGON, numVertices, numVertices));

  //retessellator to convert concave contour
  osgUtil::Tessellator tessellator;
  tessellator.setTessellationType(osgUtil::Tessellator::TESS_TYPE_POLYGONS);
  tessellator.setWindingType(osgUtil::Tessellator::TESS_WINDING_ODD);
  tessellator.retessellatePolygons(*extrusion);

  //create side quad strip indices after tessellator which might add
  //or remove primitive
  osg::ref_ptr<osg::DrawElementsUInt> sideIndices =
    new osg::DrawElementsUInt(GL_QUAD_STRIP);
  for (unsigned int i = 0; i < numVertices; ++i) {
    sideIndices->push_back(i);
    sideIndices->push_back((numVertices - 1 - i) + numVertices);
  }
  //add first column to finish circle
  sideIndices->push_back(0);
  sideIndices->push_back(numVertices * 2 - 1);
  extrusion->addPrimitiveSet(sideIndices.get());

  osgUtil::SmoothingVisitor::smooth(*extrusion);
  return extrusion.release();
}

int main(int argc, char* argv[]) {
  //read arguments
  osg::ArgumentParser arguments(&argc, argv);
  osg::Vec3 direction(0.0f, 0.0f, -1.0f);
  arguments.read("--direction", direction.x(), direction.y(), direction.z());
  float length = 5.0f;
  arguments.read("--length", length);

  //create 2d contour
  osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array(6);
  (*vertices)[0].set(0.0f, 4.0f, 0.0f);
  (*vertices)[1].set(-2.0f, 5.0f, 0.0f);
  (*vertices)[2].set(-5.0f, 0.0f, 0.0f);
  (*vertices)[3].set(0.0f, -1.0f, 0.0f);
  (*vertices)[4].set(5.0f, 0.0f, 0.0f);
  (*vertices)[5].set(2.0f, 5.0f, 0.0f);

  osg::ref_ptr<osg::Geode> geode = new osg::Geode;
  geode->addDrawable(createExtrusion(vertices.get(), direction, length));

  osg::ref_ptr<osg::Group> root = new osg::Group;
  root->addChild(geode.get());

  osgViewer::Viewer viewer;
  viewer.setSceneData(root.get());
  return viewer.run();
}
