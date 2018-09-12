#include <osgViewer/Viewer>
#include <osg/Math>
#include <zmath.h>
#include <osg/Program>
#include <iostream>
#include <osg/Texture2D>
#include <osgDB/ReadFile>
#include <osg/BlendFunc>

#define NUM_STARS 50000

class Star
{
public:
  GLfloat x, y;
  GLfloat z; // z will be used to move the star
  GLfloat m_speed;
  Star()
  {
    reset();
  }

  void update(GLfloat dt)
  {
    z -= 1 * dt * m_speed;
    if(z < 1)
    {
      reset();
    }
  }

  void reset()
  {
    x = zxd::Math::randomValue(-1000, 1000);
    y = zxd::Math::randomValue(-1000, 1000);
    z = std::max(1000, 1000);
    m_speed = zxd::Math::randomValue(0.01, 1.0) * 2000;
  }

  osg::Vec3 normalize()
  {
    //GLfloat nx = x / z;
    //GLfloat ny = y / z;
    //return vec2(nx, ny);
    return osg::Vec3(x, y, 1/z);
  }
};

typedef std::vector<Star> Stars;
Stars stars;
osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array();

class StarCallback : public osg::NodeCallback {
    virtual void operator()(osg::Node* node, osg::NodeVisitor* nv) {

      static GLdouble lastTime = nv->getFrameStamp()->getReferenceTime();
      GLdouble currentTime = nv->getFrameStamp()->getReferenceTime();
      GLdouble deltaTime = currentTime - lastTime;
      lastTime = nv->getFrameStamp()->getReferenceTime();

      vertices->dirty();

      for (unsigned i = 0; i < stars.size(); ++i) {
        stars[i].update(deltaTime);
        vertices->at(i) = stars[i].normalize();
      }

      traverse(node,nv);
    }
};

static const char* vsSource = 
"#version 120                                             \n "
"                                                         \n "
"void main(void)                                          \n "
"{                                                        \n "
"  gl_Position = vec4(gl_Vertex.xy * gl_Vertex.z, 0, 1);  \n "
"  gl_PointSize = gl_Vertex.z * 3000.0;                   \n "
"}                                                        \n "
;

static const char* fsSource = 
"#version 120                                            \n "
"                                                        \n "
"uniform sampler2D diffuse_map;                          \n "
"                                                        \n "
"void main(void)                                         \n "
"{                                                       \n "
"  gl_FragColor = texture2D(diffuse_map, gl_PointCoord); \n "
"}                                                       \n "
;

int main(int argc, char *argv[])
{
  osg::ref_ptr<osg::Geode> scene  = new osg::Geode();
  osg::ref_ptr<osg::Geometry> geometry  = new osg::Geometry();

  stars.resize(NUM_STARS);
  vertices->resize(NUM_STARS);

  geometry->setVertexArray(vertices);
  geometry->addPrimitiveSet(new osg::DrawArrays(GL_POINTS, 0, vertices->size()));

  geometry->setUseDisplayList(false);
  geometry->setDataVariance(osg::Object::DYNAMIC);

  geometry->setUpdateCallback(new StarCallback);

  osg::ref_ptr<osg::Program> program  = new osg::Program();
  program->addShader(new osg::Shader(osg::Shader::VERTEX, vsSource));
  program->addShader(new osg::Shader(osg::Shader::FRAGMENT, fsSource));

  osg::StateSet* ss = geometry->getOrCreateStateSet();
  ss->setAttribute(program);
  ss->setMode(GL_PROGRAM_POINT_SIZE, osg::StateAttribute::ON);
  ss->setMode(GL_POINT_SPRITE, osg::StateAttribute::ON); // you need to enable point sprite in gl 2
  ss->addUniform(new osg::Uniform("diffuse_map", 0));

  osg::ref_ptr<osg::Texture2D> texture  = new osg::Texture2D();
  osg::ref_ptr<osg::Image> image = osgDB::readImageFile("Images/flare.png");
  texture->setImage(image);
  ss->setTextureAttributeAndModes(0, texture);

  osg::ref_ptr<osg::BlendFunc> blendFunc  = new osg::BlendFunc(GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR);
  ss->setAttributeAndModes(blendFunc);
  ss->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

  scene->addDrawable(geometry);

  osgViewer::Viewer viewer;
  viewer.setSceneData(scene);
  return viewer.run();
}
