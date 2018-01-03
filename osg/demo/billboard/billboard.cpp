#include <osg/Billboard>
#include <osg/Texture2D>
#include <osgDB/ReadFile>
#include <osgViewer/Viewer>

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
  //simply transform -y(billoard normal) to ev
  geode->setMode(osg::Billboard::POINT_ROT_WORLD);

  osg::Geometry* quad = createQuad();

  for (unsigned int i = 0; i < 10; ++i) {
    geode->addDrawable(quad, osg::Vec3(-2.5f + 0.2f * i, i, 0.0f));
    geode->addDrawable(quad, osg::Vec3(2.5f - 0.2f * i, i, 0.0f));

    // All quad textures' backgrounds are automatically cleared because of the
    // alpha test, which is performed internally in the osgdb_png plugin. That
    // means we have to set correct rendering order of all the drawables to
    // ensure that the entire process is working properly

    osg::StateSet* ss = geode->getOrCreateStateSet();
    ss->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
  }

  osgViewer::Viewer viewer;
  viewer.setSceneData(geode.get());
  return viewer.run();
}
