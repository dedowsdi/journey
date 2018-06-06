#include <osg/Billboard>
#include <osg/Texture2D>
#include <osgDB/ReadFile>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osg/Math>
#include <osg/MatrixTransform>
#include <osg/Matrix>

#define NUM_TREES 300
#define GROUND_WIDTH 100
#define GROUND_HEIGHT 100
#define MIN_SCALE 0.5
#define MAX_SCALE 5.5

GLfloat random(GLfloat min, GLfloat max) {
  return min + static_cast<GLfloat>(rand()) / RAND_MAX * (max - min);
}

osg::Geometry* createQuad(GLfloat width, GLfloat height) {
  osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
  osg::ref_ptr<osg::Image> image = osgDB::readImageFile("Images/tree0.rgba");
  texture->setImage(image.get());

  osg::ref_ptr<osg::Geometry> quad =
    osg::createTexturedQuadGeometry(osg::Vec3(-0.5f, 0.0f, -0.5f),
      osg::Vec3(width, 0.0f, 0.0f), osg::Vec3(0.0f, 0.0f, height));

  osg::StateSet* ss = quad->getOrCreateStateSet();
  ss->setTextureAttributeAndModes(0, texture.get());
  return quad.release();
}

int main(int argc, char* argv[]) {
  osg::ref_ptr<osg::Group> root = new osg::Group();
  // create billboards with different transform
  for (GLuint i = 0; i < NUM_TREES; i += NUM_TREES / 20) {
    osg::ref_ptr<osg::Billboard> bb = new osg::Billboard();
    // face camera,  keep z up
    bb->setMode(osg::Billboard::POINT_ROT_EYE);

    // shift normal a little bit around 0 -1 0
    bb->setNormal(osg::Vec3(random(-0.1f, 0.1f), -1.0f, random(-0.1f, 0.1f)));

    osg::StateSet* ss = bb->getOrCreateStateSet();
    ss->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

    osg::Geometry* quad = createQuad(random(1, 5), random(0.5, 10));

    for (unsigned int j = 0; j < NUM_TREES / 10; ++j) {
      bb->addDrawable(quad,
        osg::Vec3(random(0, GROUND_WIDTH), random(0, GROUND_HEIGHT), 0.0f));
      bb->addDrawable(quad,
        osg::Vec3(random(0, GROUND_WIDTH), random(0, GROUND_HEIGHT), 0.0f));

    }

    // don't use MatrixTransform to scale billboard, it will break it.
    //osg::ref_ptr<osg::MatrixTransform> mt = new osg::MatrixTransform();
    //mt->setMatrix(osg::Matrix::scale(osg::Vec3(
      //random(0.2f, 5.0f), random(0.2f, 5.0f), random(0.2f, 5.0f))));
    //mt->addChild(bb);

    //root->addChild(mt);
    root->addChild(bb);
  }

  osgViewer::Viewer viewer;
  viewer.setSceneData(root.get());
  viewer.addEventHandler(new osgViewer::StatsHandler);
  return viewer.run();
}
