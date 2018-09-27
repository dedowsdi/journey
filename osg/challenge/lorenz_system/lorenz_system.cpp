#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgUtil/PrintVisitor>

osg::ref_ptr<osg::Group> root;
osg::ref_ptr<osg::Geometry> geometry;

GLfloat timeStep = 0.01;
GLuint count = 50000;
GLuint drawCount = 2;
GLuint drawStep = 3; // increment per frame
GLboolean dirty = GL_FALSE;
osg::BoundingBox bb;

struct Setting
{
  osg::Vec3 start = osg::Vec3(0.01, 0, 0);
  GLfloat sigma = 10;
  GLfloat rho =  28;
  GLfloat beta = 8.0/3;
} s;

void updateBuffer();
void updateDrawCount();

struct ComputeLorenzBoundingBox : public osg::Drawable::ComputeBoundingBoxCallback 
{
  virtual osg::BoundingBox computeBound(const osg::Drawable&) const
  { 
    return bb;
  }
};


void createGeometry()
{
  geometry = new osg::Geometry;
  osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array();
  osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array(osg::Array::BIND_PER_VERTEX);
  
  geometry->setVertexArray(vertices);
  geometry->setColorArray(colors);
  geometry->setUseDisplayList(false);
  geometry->setDataVariance(osg::Object::DYNAMIC);

  //osg::StateSet* ss = geometry->getOrCreateStateSet();
  geometry->addPrimitiveSet(new osg::DrawArrays(GL_LINE_STRIP, 0, drawCount));
  geometry->setComputeBoundingBoxCallback(new ComputeLorenzBoundingBox());

  updateBuffer();
}

void updateBuffer()
{
  drawCount = 2;
  osg::Vec3Array* vertices = static_cast<osg::Vec3Array*>(geometry->getVertexArray());
  osg::Vec4Array* colors = static_cast<osg::Vec4Array*>(geometry->getColorArray());

  if (vertices->size() != count) 
  {
    vertices->resize(count);
    colors->resize(count);
  }

  vertices->at(0) = s.start;
  colors->at(0) = osg::Vec4(0,0,0,0);
  bb.expandBy(s.start);

  for (size_t i = 1; i < count; ++i) 
  {
    const osg::Vec3& p = (*vertices)[i-1];
    (*vertices)[i] = p;
    bb.expandBy(p);

    (*vertices)[i].x() += timeStep * s.sigma*(p.y() - p.x());
    (*vertices)[i].y() += timeStep * (p.x() * (s.rho - p.z()) - p.y());
    (*vertices)[i].z() += timeStep * (p.x() * p.y() - s.beta * p.z());

    (*colors)[i] = osg::Vec4((i&0xff)/255.0, (i&0x7f)/127.0, (i&0x3f)/63.0, 1);
  }


  vertices->dirty();
  colors->dirty();
  geometry->dirtyBound();
}

void updateDrawCount()
{
  drawCount += drawStep;
  drawCount = std::min(drawCount, 50000u);
  osg::DrawArrays* da = static_cast<osg::DrawArrays*>(geometry->getPrimitiveSet(0));
  da->setCount(drawCount);
  geometry->dirtyBound();
  //OSG_WARN << da->getCount() << std::endl;
}

class RootUpdate : public osg::NodeCallback
{
  virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
  {
    updateDrawCount();
    traverse(node,nv);
  }
};


int main(int argc, char* argv[])
{
  createGeometry();

  root = new osg::Group;

  osg::StateSet* ss = root->getOrCreateStateSet();
  ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
  ss->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);

  root->addUpdateCallback(new RootUpdate);
  
  osg::ref_ptr<osg::Geode> leaf  = new osg::Geode();
  leaf->addDrawable(geometry);
  root->addChild(leaf);
  
  osgViewer::Viewer viewer;
  viewer.addEventHandler(new osgViewer::StatsHandler);
  viewer.setSceneData(root);
  
  return viewer.run();
}
