#include <osg/Geometry>
#include <osg/Geode>
#include <osg/Texture2D>
#include <osgDB/ReadFile>
#include <osgViewer/Viewer>

#include "common.h"

#define WIDTH 384
#define HALF_HEIGHT 64 


const char* vertCode = {
  "uniform sampler2D defaultTex;                                                             \n "
  "uniform vec4 wh; //width and half height                                                  \n "
  "const float PI2 = 6.2831852;                                                              \n "
  "void main()                                                                               \n "
  "{                                                                                         \n "
  "    float r = float(gl_InstanceID) / 256.0;                                               \n "
  "    vec2 uv = vec2(fract(r), floor(r) / 256.0); //normalized col , row                    \n "
  "    //create a wave on y direction                                                        \n "
  "    vec4 pos = gl_Vertex + vec4(uv.s * wh.x, wh.y * sin(uv.s * PI2), uv.t * wh.x, 1.0); \n "
  "    gl_FrontColor = texture2D(defaultTex, uv);                                            \n "
  "    gl_Position = gl_ModelViewProjectionMatrix * pos;                                     \n "
  "}                                                                                         \n "};

osg::Geometry* createInstancedGeometry(unsigned int numInstances) {
  //create a quad
  osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array(4);
  (*vertices)[0].set(-0.5f, 0.0f, -0.5f);
  (*vertices)[1].set(0.5f, 0.0f, -0.5f);
  (*vertices)[2].set(0.5f, 0.0f, 0.5f);
  (*vertices)[3].set(-0.5f, 0.0f, 0.5f);

  osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
  geom->setUseDisplayList(false);
  geom->setUseVertexBufferObjects(true);
  geom->setVertexArray(vertices.get());
  geom->addPrimitiveSet(new osg::DrawArrays(GL_QUADS, 0, 4, numInstances));
  //set up a customized bounding box again as the system can't decide the actual
  //bound according to only four original points
  geom->setInitialBound(
    osg::BoundingBox(-1.0f, -HALF_HEIGHT, -1.0f, WIDTH/2, HALF_HEIGHT, WIDTH/2));

  osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
  texture->setImage(osgDB::readImageFile("Images/osg256.png"));
  texture->setFilter(osg::Texture2D::MIN_FILTER, osg::Texture2D::LINEAR);
  texture->setFilter(osg::Texture2D::MAG_FILTER, osg::Texture2D::LINEAR);
  geom->getOrCreateStateSet()->setTextureAttributeAndModes(0, texture.get());
  geom->getOrCreateStateSet()->addUniform(new osg::Uniform("defaultTex", 0));
  geom->getOrCreateStateSet()->addUniform(new osg::Uniform("wh", osg::Vec4(WIDTH, HALF_HEIGHT,0.0f, 0.0f)));

  osg::ref_ptr<osg::Program> program = new osg::Program;
  program->addShader(new osg::Shader(osg::Shader::VERTEX, vertCode));
  geom->getOrCreateStateSet()->setAttributeAndModes(program.get());
  return geom.release();
}

int main(int argc, char** argv) {
  osg::ref_ptr<osg::Geode> geode = new osg::Geode;
  geode->addDrawable(createInstancedGeometry(256 * 256));

  osg::ref_ptr<osg::Group> root = new osg::Group;
  root->addChild(geode.get());

  osgViewer::Viewer viewer;
  viewer.setSceneData(root.get());
  return viewer.run();
}
