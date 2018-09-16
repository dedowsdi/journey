/*
 * purple rain. All rain drops are created in 1 single geometry.
 */
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osg/MatrixTransform>
#include <osgUtil/PrintVisitor>
#include <iostream>
#include "zmath.h"

#define RAIN_COLOR (osg::Vec3(138, 43, 226)/255.0f)
//#define BG_COLOR (osg::Vec4(230, 230, 250, 256)/255.0f)
#define BG_COLOR (osg::Vec4(0,0,0,1))
#define NUM_DROPS 300000
#define RADIUS 2000.0f

// fix bound
struct RainComputeBoundCallback : public osg::Drawable::ComputeBoundingBoxCallback 
{
  virtual osg::BoundingBox computeBound(const osg::Drawable&) const
  {
    return osg::BoundingBox(-RADIUS, -RADIUS, -100, RADIUS, RADIUS, RADIUS*2); 
  }
};


osg::ref_ptr<osg::Geometry> rainGeometry;
void createRainGeometry()
{
  rainGeometry = new osg::Geometry();
  rainGeometry->setComputeBoundingBoxCallback(new RainComputeBoundCallback());

  osg::ref_ptr<osg::Vec3Array> vertices  = new osg::Vec3Array(osg::Array::BIND_PER_VERTEX);
  osg::ref_ptr<osg::Vec3Array> colors  = new osg::Vec3Array(osg::Array::BIND_OVERALL);

  vertices->reserve(NUM_DROPS*2);
  for (size_t i = 0; i < NUM_DROPS; ++i) 
  {
    vertices->push_back(osg::Vec3(0,0,0));
    vertices->push_back(osg::Vec3(0,0,-1));
  }
  colors->push_back(RAIN_COLOR);

  rainGeometry->setVertexArray(vertices);
  rainGeometry->setColorArray(colors);

  rainGeometry->addPrimitiveSet(new osg::DrawArrays(GL_LINES, 0, NUM_DROPS * 2));
  rainGeometry->setUseDisplayList(false);
  rainGeometry->setUseVertexBufferObjects(true);
  rainGeometry->setDataVariance(osg::Object::DYNAMIC);
}

struct RainDrop
{
  GLfloat zspeed;
  GLfloat accel;
  GLfloat size;
  osg::Vec3 pos;

  RainDrop()
  {
    reset();
  }

  void update(GLfloat dt)
  {
    pos.z() += zspeed * dt;
    zspeed += accel * dt;
    if(pos.z() < 0)
      reset();
  }

  void reset()
  {
    zspeed = zxd::Math::randomValue(0.25f, 1.0f) * -50;
    accel = zxd::Math::randomValue(0.0f, 1.0f) * -200;
    size = zxd::Math::randomValue(2.0f, 15.0f);

    pos.x() = zxd::Math::randomValue(-RADIUS, RADIUS);
    pos.y() = zxd::Math::randomValue(-RADIUS, RADIUS);
    pos.z() = zxd::Math::randomValue(RADIUS*1.5, RADIUS*1.8);
  }
};

struct rain : public osg::Group
{
  std::vector<RainDrop> mRainDrops;
  rain()
  {
    mRainDrops.resize(NUM_DROPS);

    getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
    osg::ref_ptr<osg::Geode> leaf  = new osg::Geode();
    addChild(leaf);
    leaf->addDrawable(rainGeometry);
  }

  void update(GLfloat dt)
  {
    osg::Vec3Array* vertices = static_cast<osg::Vec3Array*>(rainGeometry->getVertexArray());
    for (size_t i = 0; i < mRainDrops.size(); ++i) {
      RainDrop& rd = mRainDrops[i];
      rd.update(dt);
      vertices->at(i*2) = rd.pos;
      vertices->at(i*2+1) = rd.pos + osg::Vec3(0,0,-rd.size);
    }
    vertices->dirty();
    rainGeometry->dirtyBound();
  }
};

class RainUpdateCallback : public osg::NodeCallback {
protected:
  osg::ref_ptr<rain> mRain;

public:
  RainUpdateCallback(rain* r):
    mRain(r)
  {
  }

protected:
  virtual void operator()(osg::Node* node, osg::NodeVisitor* nv) {

    static GLfloat lastTime = nv->getFrameStamp()->getReferenceTime();
    GLfloat currentTime = nv->getFrameStamp()->getReferenceTime();
    GLfloat deltaTime = currentTime - lastTime;
    mRain->update(deltaTime);

    lastTime = currentTime;
    traverse(node,nv);
  }

};

int main(int argc, char *argv[])
{
  createRainGeometry();
  osg::ref_ptr<rain> rain0 = new rain();
  rain0->setInitialBound(osg::BoundingBox(-RADIUS, -RADIUS, -100, RADIUS, RADIUS, RADIUS*2)); 
  //rain0->setBound(osg::BoundingBox(-RADIUS, -RADIUS, -100, RADIUS, RADIUS, RADIUS*2)); 
  rain0->addUpdateCallback(new RainUpdateCallback(rain0));

  osgViewer::Viewer viewer;
  viewer.setSceneData(rain0);

  viewer.addEventHandler(new osgViewer::StatsHandler);
  viewer.getCamera()->setClearColor(BG_COLOR);

  osg::ref_ptr<osgUtil::PrintVisitor> pv  = new osgUtil::PrintVisitor(std::cout);
  rain0->accept(*pv);

  return viewer.run();
}
