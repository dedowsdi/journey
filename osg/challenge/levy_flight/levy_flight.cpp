#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgUtil/PrintVisitor>
#include "glm.h"

osg::ref_ptr<osg::Group> root;
osg::ref_ptr<osg::Geometry> line;

void createLine()
{
  line = new osg::Geometry;
  osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array();
  osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array(osg::Array::BIND_OVERALL);
  
  line->setVertexArray(vertices);
  line->setColorArray(colors);

  vertices->push_back(osg::Vec3());
  vertices->push_back(osg::Vec3());
  colors->push_back(osg::Vec4(1,1,1,1));
  
  line->addPrimitiveSet(new osg::DrawArrays(GL_LINES, 0, 2));

  line->setUseDisplayList(false);

  line->setInitialBound(osg::BoundingBox(osg::Vec3(-100,-100,-100), osg::Vec3(100,100,100)));
  
  //osg::StateSet* ss = line->getOrCreateStateSet();
}

void updateLine()
{
  auto vertices = static_cast<osg::Vec3Array*>(line->getVertexArray());
  vertices->at(0) = vertices->at(1);
  vertices->at(1) = glm::sphericalRand(1.0f);
  if(glm::linearRand(0.0f, 1.0f) < 0.02)
    vertices->at(1) *= 20.0f;
  vertices->at(1) += vertices->at(0);

  vertices->dirty();
  line->dirtyBound();
}


class RootUpdate : public osg::NodeCallback
{
  virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
  {
    updateLine();
    traverse(node,nv);
  }
};

int main(int argc, char* argv[])
{
  root = new osg::Group;
  root->addUpdateCallback(new RootUpdate);

  osg::StateSet* ss = root->getOrCreateStateSet();
  ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
  ss->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);

  createLine();
  osg::ref_ptr<osg::Geode> leaf = new osg::Geode();
  leaf->addDrawable(line);
  root->addChild(leaf);
  
  osgViewer::Viewer viewer;
  viewer.addEventHandler(new osgViewer::StatsHandler);
  viewer.setSceneData(root);
  viewer.realize();

  osg::ref_ptr<osgUtil::PrintVisitor> pv = new osgUtil::PrintVisitor(osg::notify(osg::NOTICE));
  root->accept(*pv);

  // no clear, no camera manipulatosg::ref_ptr<>
  osg::Camera* camera = viewer.getCamera();
  camera->setAllowEventFocus(false);
  camera->setClearMask(0);
  camera->setViewMatrixAsLookAt(osg::Vec3(0, -300, 300), osg::Vec3(0,0,0), glm::pza);
  
  return viewer.run();
}
