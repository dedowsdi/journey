#include <osg/Geometry>
#include <osg/Geode>
#include <osg/Depth>
#include <osg/Texture2D>
#include <osgDB/ReadFile>
#include <osgViewer/Viewer>
#include "common.h"

int main(int argc, char** argv) {
  // load background image to texture
  osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
  osg::ref_ptr<osg::Image> image = osgDB::readImageFile("Images/osg256.png");
  texture->setImage(image.get());

  GLuint width = 800, height = 600;
  zxd::getScreenResolution(width, height);

  // create a quad to place texture
  osg::ref_ptr<osg::Drawable> quad = osg::createTexturedQuadGeometry(
    osg::Vec3(), osg::Vec3(width, 0.0f, 0.0f), osg::Vec3(0.0f, height, 0.0f));
  quad->getOrCreateStateSet()->setTextureAttributeAndModes(0, texture.get());

  osg::ref_ptr<osg::Geode> geode = new osg::Geode;
  geode->addDrawable(quad.get());

  //this is not a standard hud camera, it's background camera!, it clears
  //nothing.
  osg::Camera* hudCamera = zxd::createHUDCamera();
  hudCamera->setClearMask(0);

  hudCamera->addChild(geode);

  osg::StateSet* ss = hudCamera->getOrCreateStateSet();
  ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
  //clamp depth to 1, so depth test will always fail if there is something
  ss->setAttributeAndModes(new osg::Depth(osg::Depth::LEQUAL, 1.0, 1.0));

  osg::ref_ptr<osg::Group> root = new osg::Group;
  root->addChild(hudCamera);
  root->addChild(osgDB::readNodeFile("cessna.osg"));

  osgViewer::Viewer viewer;
  viewer.setSceneData(root.get());
  return viewer.run();
}
