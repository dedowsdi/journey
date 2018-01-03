#include <osg/LOD>
#include <osg/PagedLOD>
#include <osgDB/ReadFile>
#include <osgUtil/Simplifier>
#include <osgViewer/Viewer>

/*
 * auto lod ????
 */

int main(int argc, char *argv[]) {
  osg::ref_ptr<osg::Node> modelL1 = osgDB::readNodeFile("cessna.osg");

  osgUtil::Simplifier simplifier;
  simplifier.setSampleRatio(0.1);
  modelL1->accept(simplifier);

  osg::ref_ptr<osg::PagedLOD> pagedLOD = new osg::PagedLOD;
  pagedLOD->addChild(modelL1, 200.0f, FLT_MAX);
  pagedLOD->setFileName(1, "cessna.osg");
  pagedLOD->setRange(1, 0.0f, 200.0f);

  osgViewer::Viewer viewer;
  viewer.setSceneData(pagedLOD.get());
  return viewer.run();

}
