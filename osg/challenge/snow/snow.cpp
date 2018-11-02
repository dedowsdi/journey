#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgUtil/PrintVisitor>
#include <osg/Billboard>
#include <osgDB/ReadFile>
#include <osg/BlendFunc>
#include "streamutil.h"
#include "glm.h"

GLint numFlakes = 10000;
osg::ref_ptr<osg::Group> root;
struct Snow;
std::vector<osg::ref_ptr<Snow>> snows;
osg::ref_ptr<osg::Texture2D> texture;
osg::ref_ptr<osg::Billboard> billboard;

struct Snow : public osg::Geometry
{
  // buffer data
  float angle = 0;
  osg::Vec2 texcoord = osg::Vec2(); // lb corner
  osg::Vec2 size = osg::Vec2(10, 10);
  osg::Vec3 pos;

  // following data will be copied into vertex buffer, should i collect them to
  // something like snow_property?
  GLfloat angular_velocity = 0;
  osg::Vec3 vel = osg::Vec3();
  osg::Vec3 acc = osg::Vec3();

  Snow(const osg::Vec3& pos_, const osg::Vec2& size_, float angle_,
      const osg::Vec2& texCorner, const osg::Vec2& tileSize):
    angle(angle_),
    size(size_),
    pos(pos_)
  {
    osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array();
    osg::ref_ptr<osg::Vec2Array> texcoords = new osg::Vec2Array(osg::Array::BIND_PER_VERTEX);
    setVertexArray(vertices);
    setTexCoordArray(0, texcoords);

    vertices->push_back(osg::Vec3(-size.x(), 0, -size.y()));
    vertices->push_back(osg::Vec3(size.x(),  0, -size.y()));
    vertices->push_back(osg::Vec3(size.x(),  0, size.y()));
    vertices->push_back(osg::Vec3(-size.x(), 0, size.y()));

    texcoords->push_back(texCorner);
    texcoords->push_back(texCorner + osg::Vec2(tileSize.x(), 0));
    texcoords->push_back(texCorner + tileSize);
    texcoords->push_back(texCorner + osg::Vec2(0, tileSize.y()));

    addPrimitiveSet(new osg::DrawArrays(GL_TRIANGLE_FAN, 0, 4));
  }

  void update()
  {
    pos += vel;
    vel += acc;
    angle += angular_velocity;
    acc = osg::Vec3();

    if(pos.z() < 0)
    {
      pos.z() = 500;
      random();
    }
  }

  void applyForce(const osg::Vec3& f)
  {
    acc += f;
  }

  void random()
  {
    vel = glm::linearRand(osg::Vec3(-1, -1, -1), osg::Vec3(1, 1, -1));
    angle = glm::linearRand(0.0f, 1.0f) * glm::f2pi;
    angular_velocity = glm::linearRand(-0.03, 0.03);
  }
};

void createSnows()
{
  billboard = new osg::Billboard;
  root->addChild(billboard);

  texture = new osg::Texture2D;
  osg::Texture2D* texture = new osg::Texture2D;
  texture->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
  texture->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
  texture->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR);
  texture->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR);
  osg::ref_ptr<osg::Image> image = osgDB::readImageFile("data/texture/f32.png");
  texture->setImage(image);

  osg::StateSet* ss = billboard->getOrCreateStateSet();
  ss->setTextureAttributeAndModes(0, texture);
  ss->setAttributeAndModes(new osg::BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
  ss->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

  GLuint rows = 16;
  GLuint cols = 16;
  osg::Vec2 tileSize = osg::Vec2(1.0/cols, 1.0/rows);

  snows.reserve(numFlakes);
  for (int i = 0; i < numFlakes; ++i) 
  {
    osg::Vec3 pos = glm::linearRand(osg::Vec3(-500, -500, 10), osg::Vec3(500, 500, 500));
    float size = glm::linearRand(3.0, 6.0);
    float angle = glm::linearRand(0.0f, glm::f2pi);
    osg::Vec2 texCorner = componentMultiply(tileSize,
        glm::floor(glm::linearRand(osg::Vec2(), osg::Vec2(cols, rows))));
    //OSG_NOTICE << texCorner << std::endl;
    osg::ref_ptr<Snow> s = new Snow(pos, osg::Vec2(size, size), angle, texCorner, tileSize);
    s->random();
    snows.push_back(s);
    billboard->addDrawable(s, s->pos);
  }
}

class RootUpdate : public osg::NodeCallback
{
  virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
  {
    for (int i = 0; i < snows.size(); ++i) {
      Snow* flake = snows[i];
      flake->applyForce(osg::Vec3(0, 0, -0.02));
      flake->update();
      billboard->setPosition(i, flake->pos);
    }
    traverse(node,nv);
  }
};

int main(int argc, char* argv[])
{
  srand(time(0));
  root = new osg::Group;
  createSnows();

  root->addUpdateCallback(new RootUpdate);

  osg::StateSet* ss = root->getOrCreateStateSet();
  ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
  //ss->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);

  osgViewer::Viewer viewer;
  viewer.addEventHandler(new osgViewer::StatsHandler);
  viewer.setSceneData(root);
  viewer.setLightingMode(osg::View::NO_LIGHT);

  osg::Camera* camera = viewer.getCamera();
  camera->setClearColor(osg::Vec4());

  osg::ref_ptr<osgUtil::PrintVisitor> pv = new osgUtil::PrintVisitor(osg::notify(osg::NOTICE));
  root->accept(*pv);

  return viewer.run();
}
