/*
 * there are 3 kins of billboard:
 *   point_rot_eye : look at camera from billboard, use camera up as up in lookat
 *   point_rot_world: rotate normal to center to eye, don't care about orientation
 *   axial_rot : rotate around certain axis
 *
 * billboard should be created in xz plane, use (0,-1,0) as normal
 *
 * billboard is derived from geode, you can only add drawable to it!!
 */
#include <osg/Billboard>
#include <osg/Texture2D>
#include <osgDB/ReadFile>
#include <osgViewer/Viewer>
#include <osg/MatrixTransform>

osg::Geometry* createQuad() {
  osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
  osg::ref_ptr<osg::Image> image = osgDB::readImageFile("Images/osg256.png");
  texture->setImage(image.get());

  osg::ref_ptr<osg::Geometry> quad =
    osg::createTexturedQuadGeometry(osg::Vec3(-0.5f, 0.0f, -0.5f),
      osg::Vec3(1.0f, 0.0f, 0.0f), osg::Vec3(0.0f, 0.0f, 1.0f));

  osg::StateSet* ss = quad->getOrCreateStateSet();
  ss->setTextureAttributeAndModes(0, texture.get());
  return quad.release();
}

int main(int argc, char* argv[]) {
  osg::ref_ptr<osg::Billboard> geode = new osg::Billboard;
  geode->setMode(osg::Billboard::POINT_ROT_WORLD);

  osg::Geometry* quad = createQuad();

  osg::ref_ptr<osg::Billboard> geode1 = new osg::Billboard;
  geode1->setMode(osg::Billboard::POINT_ROT_WORLD);


  for (unsigned int i = 0; i < 10; ++i) {
    geode->addDrawable(quad, osg::Vec3(-2.5f + 0.2f * i, i, 0.0f));
    geode->addDrawable(quad, osg::Vec3(2.5f - 0.2f * i, i, 0.0f));
  }

  // All quad textures' backgrounds are automatically cleared because of the
  // alpha test, which is performed internally in the osgdb_png plugin. That
  // means we have to set correct rendering order of all the drawables to
  // ensure that the entire process is working properly
  osg::StateSet* ss = geode->getOrCreateStateSet();
  ss->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

  osgViewer::Viewer viewer;
  viewer.setSceneData(geode);
  return viewer.run();
}
