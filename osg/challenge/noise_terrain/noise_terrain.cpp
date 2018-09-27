#include <osg/PolygonMode>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgUtil/PrintVisitor>
#include <osgUtil/PerlinNoise>
#include <iostream>

#define TERRAIN_WIDTH 100.0f
#define TERRAIN_HEIGHT 100.0f
#define NUM_ROWS 80
#define NUM_COLS 80
#define NOISE_STEP 0.05
#define NOISE_HEIGHT 15

osg::ref_ptr<osg::Group> root;
osg::ref_ptr<osg::Geometry> geometry;

void createTerrain()
{
  geometry = new osg::Geometry;
  geometry->setUseDisplayList(false);
  geometry->setDataVariance(osg::Object::DYNAMIC);

  osg::ref_ptr<osg::Vec3Array> vertices  = new osg::Vec3Array(osg::Array::BIND_PER_VERTEX);
  geometry->setVertexArray(vertices);

  GLfloat row_step = TERRAIN_HEIGHT  / NUM_ROWS;
  GLfloat col_step = TERRAIN_WIDTH / NUM_COLS;

  osg::Vec3 start_pos(-row_step * NUM_ROWS * 0.5, -col_step * NUM_COLS * 0.5, 0);
  for (int i = 0; i < NUM_ROWS; ++i) {
    GLfloat y1 = i * row_step;
    GLfloat y0 = y1 + row_step;

    for (int j = 0; j <= NUM_COLS; ++j) {
      GLfloat x = col_step * j;
      vertices->push_back(osg::Vec3(x, y0, 0) + start_pos);
      vertices->push_back(osg::Vec3(x, y1, 0) + start_pos);
    }
  }

    GLint next = 0;
    GLint count = (NUM_COLS + 1) * 2;
    for (int i = 0; i < NUM_ROWS; ++i) {
      geometry->addPrimitiveSet(new osg::DrawArrays(GL_TRIANGLE_STRIP, next, count));
      next +=  count;
    }

}

void updateBuffer(const osg::Vec2d& noiseOffset)
{
  osg::Vec3Array* vertices = static_cast<osg::Vec3Array*>(geometry->getVertexArray());
  osg::Vec2d noisePos = noiseOffset;
  GLint rowSize = (NUM_COLS + 1) * 2;

  osgUtil::PerlinNoise pn;

  pn.SetNoiseFrequency(4);

  for (int i = 0; i < NUM_ROWS; ++i) {

    noisePos.y() += NOISE_STEP ;
    noisePos.x()= noiseOffset.x();

    GLint row_start = rowSize * i;

    for (int j = 0; j <= NUM_COLS; ++j) {
      noisePos.x() += NOISE_STEP;

      vertices->at(row_start + j * 2).z() = 
        NOISE_HEIGHT * pn.PerlinNoise2D(noisePos.x(), noisePos.y() + NOISE_STEP, 2, 2, 3);
      vertices->at(row_start + j * 2 + 1).z() =
        NOISE_HEIGHT * pn.PerlinNoise2D(noisePos.x(), noisePos.y(), 2, 2, 3);
    }
  }

  vertices->dirty();
  geometry->dirtyBound();
}

class RootUpdate : public osg::NodeCallback
{
  virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
  {
    updateBuffer(osg::Vec2(nv->getFrameStamp()->getReferenceTime() * 0.25, nv->getFrameStamp()->getReferenceTime() * 0.25));
    traverse(node,nv);
  }
};

int main(int argc, char* argv[])
{
  createTerrain();
  updateBuffer(osg::Vec2());

  root = new osg::Group;
  osg::StateSet* ss = root->getOrCreateStateSet();
  //ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
  ss->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
  ss->setMode(GL_CULL_FACE, osg::StateAttribute::OFF);
  ss->setAttributeAndModes(new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE));
  osg::ref_ptr<osg::Geode> leaf  = new osg::Geode();
  leaf->addDrawable(geometry);
  root->addChild(leaf);

  root->addUpdateCallback(new RootUpdate());
  
  osgViewer::Viewer viewer;
  viewer.addEventHandler(new osgViewer::StatsHandler);
  viewer.setSceneData(root);

  osgUtil::PrintVisitor pv(std::cout);
  root->accept(pv);
  
  return viewer.run();
}
