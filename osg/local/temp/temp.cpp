#include <iostream>
#include <osg/Matrix>
#include "common.h"
#include <osg/io_utils>
#include <osgViewer/Viewer>
#include <osgGA/StandardManipulator>
#include <ctime>
#include <osgDB/ReadFile>
#include <osgGA/TrackballManipulator>
#include "common.h"
#include <osg/Geometry>
#include <osg/Point>
#include <osg/MatrixTransform>

int main(int argc, char* argv[]) {
  osgViewer::Viewer viewer;

  osg::Camera* hudCamera = zxd::createHUDCamera();

  osg::ref_ptr<osg::Geometry> gmPoint = new osg::Geometry;
  osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array();
  osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array();
  colors->setBinding(osg::Array::BIND_OVERALL);

  vertices->push_back(osg::Vec3(300.0f, 300.0f, 0.0f));
  colors->push_back(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));

  gmPoint->setVertexArray(vertices);
  gmPoint->setColorArray(colors);

  gmPoint->addPrimitiveSet(new osg::DrawArrays(GL_POINTS, 0, 1));

  osg::StateSet* ss = gmPoint->getOrCreateStateSet();
  ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
  ss->setAttributeAndModes(new osg::Point(20.0f));

  osg::ref_ptr<osg::MatrixTransform> mPointNode = new osg::MatrixTransform();
  osg::ref_ptr<osg::Geode> leaf = new osg::Geode();
  leaf->addDrawable(gmPoint);
  mPointNode->addChild(osgDB::readNodeFile("cessna.osg"));
  mPointNode->addChild(leaf);

  hudCamera->addChild(mPointNode);

  hudCamera->setCullingMode(hudCamera->getCullingMode() & ~osg::CullSettings::SMALL_FEATURE_CULLING);

  viewer.setSceneData(hudCamera);

  return viewer.run();
}
