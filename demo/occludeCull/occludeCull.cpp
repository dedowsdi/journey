#include <osg/Geometry>
#include <osg/Geode>
#include <osgViewer/ViewerEventHandlers>
#include <osgViewer/Viewer>
#include <osgUtil/Tessellator>
#include <osgUtil/SmoothingVisitor>

#define RAND(min, max) ((min) + (float)rand() / RAND_MAX * ((max) - (min)))

osg::Geode* createMassiveQuads(unsigned int number) {
  osg::ref_ptr<osg::Geode> geode = new osg::Geode;
  for (unsigned int i = 0; i < number; ++i) {
    osg::Vec3 randomCenter;
    randomCenter.x() = RAND(-100.0f, 100.0f);
    randomCenter.y() = RAND(1.0f, 100.0f);
    randomCenter.z() = RAND(-100.0f, 100.0f);
    osg::ref_ptr<osg::Drawable> quad = osg::createTexturedQuadGeometry(
      randomCenter, osg::Vec3(1.0f, 0.0f, 0.0f), osg::Vec3(0.0f, 0.0f, 1.0f));
    geode->addDrawable(quad.get());
  }
  return geode.release();
}

/*
 * occluder with holes will affect performance
 */

int main(int argc, char* argv[]) {
  osg::ref_ptr<osg::OccluderNode> occluderNode = new osg::OccluderNode;
  osg::ref_ptr<osg::ConvexPlanarOccluder> cpo = new osg::ConvexPlanarOccluder;

  // set up occluder
  cpo->getOccluder().add(osg::Vec3(-120.0f, 0.0f, -120.0f));
  cpo->getOccluder().add(osg::Vec3(120.0f, 0.0f, -120.0f));
  cpo->getOccluder().add(osg::Vec3(120.0f, 0.0f, 120.0f));
  cpo->getOccluder().add(osg::Vec3(-120.0f, 0.0f, 120.0f));
  // add holes
  osg::ConvexPlanarPolygon hole;
  hole.add(osg::Vec3(-60.0f, 0.0f, -60.0f));
  hole.add(osg::Vec3(60.0f, 0.0f, -60.0f));
  hole.add(osg::Vec3(60.0f, 0.0f, 60.0f));
  hole.add(osg::Vec3(-60.0f, 0.0f, 60.0f));
  cpo->addHole(hole);

  occluderNode->setOccluder(cpo.get());

  // draw occluder
  osg::ref_ptr<osg::Geode> occluderGeode = new osg::Geode;
  osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry();
  osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array();

  vertices->push_back(osg::Vec3(-120.0f, 0.0f, -120.0f));
  vertices->push_back(osg::Vec3(120.0f, 0.0f, -120.0f));
  vertices->push_back(osg::Vec3(120.0f, 0.0f, 120.0f));
  vertices->push_back(osg::Vec3(-120.0f, 0.0f, 120.0f));

  vertices->push_back(osg::Vec3(-60.0f, 0.0f, -60.0f));
  vertices->push_back(osg::Vec3(60.0f, 0.0f, -60.0f));
  vertices->push_back(osg::Vec3(60.0f, 0.0f, 60.0f));
  vertices->push_back(osg::Vec3(-60.0f, 0.0f, 60.0f));

  geometry->setVertexArray(vertices);
  geometry->addPrimitiveSet(new osg::DrawArrays(GL_QUADS, 0, 8));

  osg::ref_ptr<osgUtil::Tessellator> tessellator = new osgUtil::Tessellator();
  tessellator->setTessellationType(osgUtil::Tessellator::TESS_TYPE_GEOMETRY);
  tessellator->setWindingType(osgUtil::Tessellator::TESS_WINDING_ODD);
  tessellator->retessellatePolygons(*geometry);

  osgUtil::SmoothingVisitor::smooth(*geometry);

  occluderGeode->addDrawable(geometry);

  occluderNode->addChild(occluderGeode.get());

  osg::ref_ptr<osg::Group> root = new osg::Group;

  root->addChild(createMassiveQuads(100000));
  root->addChild(occluderNode.get());
  root->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

  osgViewer::Viewer viewer;
  viewer.addEventHandler(new osgViewer::StatsHandler);
  viewer.setSceneData(root.get());
  return viewer.run();
}
