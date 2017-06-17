/* -*-c++-*- OpenSceneGraph Cookbook
 * Chapter 2 Recipe 7
 * Author: Wang Rui <wangray84 at gmail dot com>
*/

#include <osg/Geometry>
#include <osg/Geode>
#include <osg/Depth>
#include <osg/Texture2D>
#include <osgDB/ReadFile>
#include <osgViewer/Viewer>

int main(int argc, char** argv) {
  // load background image to texture
  osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
  osg::ref_ptr<osg::Image> image = osgDB::readImageFile("Images/osg256.png");
  texture->setImage(image.get());

  // create a quad to place texture
  osg::ref_ptr<osg::Drawable> quad = osg::createTexturedQuadGeometry(
    osg::Vec3(), osg::Vec3(1.0f, 0.0f, 0.0f), osg::Vec3(0.0f, 1.0f, 0.0f));
  quad->getOrCreateStateSet()->setTextureAttributeAndModes(0, texture.get());

  osg::ref_ptr<osg::Geode> geode = new osg::Geode;
  geode->addDrawable(quad.get());

  // hud camera
  osg::ref_ptr<osg::Camera> camera = new osg::Camera;
  camera->setClearMask(0);
  camera->setCullingActive(false);
  camera->setAllowEventFocus(false);
  camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
  // post render to avoid unnecessary pixel draw
  camera->setRenderOrder(
    osg::Camera::POST_RENDER);  
  camera->setProjectionMatrix(osg::Matrix::ortho2D(0.0, 1.0, 0.0, 1.0));
  camera->addChild(geode.get());

  osg::StateSet* ss = camera->getOrCreateStateSet();
  ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
  //clamp depth to 1, so depth test will always fail if there is something
  ss->setAttributeAndModes(new osg::Depth(osg::Depth::LEQUAL, 1.0, 1.0));

  osg::ref_ptr<osg::Group> root = new osg::Group;
  root->addChild(camera.get());
  root->addChild(osgDB::readNodeFile("cessna.osg"));

  osgViewer::Viewer viewer;
  viewer.setSceneData(root.get());
  return viewer.run();
}
