#include <osgViewer/Viewer>
#include <osg/View>
#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <algorithm>
#include <iomanip>
#include <osgDB/ReadFile>
#include <osg/Shader>
#include <osg/Program>
#include <osg/TextureCubeMap>
#include <osg/Shape>
#include <osg/ShapeDrawable>
#include <osg/TexGen>
#include <osg/TexMat>
#include <osg/Depth>
#include <osg/NodeVisitor>

osg::Matrix getCubeStartRotate()
{
  // rotate posz to posy
  return osg::Matrix::rotate(-osg::PI_2f, osg::X_AXIS);
}

osg::TextureCubeMap* readCubeMap()
{
  auto cubemap = new osg::TextureCubeMap;
  std::array<std::string, 6> names = {"posx", "negx", "posy", "negy", "posz", "negz"};

  for (unsigned i = 0; i < 6; ++i)
  {
    cubemap->setImage(osg::TextureCubeMap::POSITIVE_X + i,
      osgDB::readImageFile("Cubemap_snow/" + names[i] + ".jpg"));
  }

  cubemap->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR);
  cubemap->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
  cubemap->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
  cubemap->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
  cubemap->setWrap(osg::Texture::WRAP_R, osg::Texture::CLAMP_TO_EDGE);

  return cubemap;
}

class SkyboxTransform : public osg::Transform
{
public:
  bool computeLocalToWorldMatrix(osg::Matrix& matrix, osg::NodeVisitor* nv) const override
  {
    if (nv && nv->getVisitorType() == osg::NodeVisitor::CULL_VISITOR)
    {
      auto local_eye = nv->getEyePoint();
      matrix.preMult(osg::Matrix::translate(local_eye));
      return true;
    }
    return false;
  }

  bool computeWorldToLocalMatrix(osg::Matrix& matrix, osg::NodeVisitor* nv) const override
  {
    if (nv && nv->getVisitorType() == osg::NodeVisitor::CULL_VISITOR)
    {
      auto local_eye = nv->getEyePoint();
      matrix.postMult(osg::Matrix::translate(-local_eye));
      return true;
    }
    return false;
  }
};

osg::Node* createSkyBox()
{
  auto sphere = new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(0, 0, 0), 1.0f));
  auto leaf = new osg::Geode;
  leaf->addDrawable(sphere);
  leaf->setCullingActive(false);

  auto root = new SkyboxTransform;
  root->addChild(leaf);

  auto ss = root->getOrCreateStateSet();

  // default object linear should be sufficient
  auto texgen = new osg::TexGen();
  ss->setTextureAttributeAndModes(0, texgen);
  auto cubemap = readCubeMap();
  ss->setTextureAttributeAndModes(0, cubemap);

  auto texmat = new osg::TexMat;
  texmat->setMatrix(osg::Matrix::inverse(osg::Matrix::rotate(-osg::PI_2f, osg::X_AXIS)));
  ss->setTextureAttribute(0, texmat);

  // miminize depth
  ss->setAttributeAndModes(new osg::Depth(osg::Depth::ALWAYS, 1.0f, 1.0f));
  ss->setRenderBinDetails(-1, "RenderBin");

  return root;
}

class CubemapCallback : public osg::NodeCallback
{
public:

  CubemapCallback(osg::TexMat* texMat):
    _texMat(texMat)
  {
  }

  virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
  {
    if (nv->getVisitorType() == osg::NodeVisitor::CULL_VISITOR)
    {
      auto cv = nv->asCullVisitor();
      if (cv)
      {
        // cubemap works on object space
        osg::Matrix rot { cv->getModelViewMatrix()->getRotate() };
        _texMat->setMatrix(osg::Matrix::inverse(getCubeStartRotate() * rot));
      }
    }
    traverse(node,nv);
  }

private:
  osg::TexMat* _texMat;

};

osg::Node* createBall()
{
  auto sphere = new osg::ShapeDrawable(new osg::Sphere(osg::Vec3(0, 0, 0), 1.0f));
  auto leaf = new osg::Geode;
  leaf->addDrawable(sphere);

  auto ss = leaf->getOrCreateStateSet();
  ss->setTextureAttributeAndModes(0, readCubeMap());

  // auto texgen = new osg::TexGen();
  // texgen->setMode(osg::TexGen::REFLECTION_MAP);
  // ss->setTextureAttributeAndModes(0, texgen);

  // auto texmat = new osg::TexMat;
  // texmat->setMatrix(osg::Matrix::inverse(osg::Matrix::rotate(-osg::PI_2f, osg::X_AXIS)));
  // ss->setTextureAttribute(0, texmat);

  auto program = new osg::Program;
  auto vs = osgDB::readShaderFile(osg::Shader::VERTEX, "shader/cubemap/cubemap.vs.glsl");
  auto fs = osgDB::readShaderFile(osg::Shader::FRAGMENT, "shader/cubemap/cubemap.fs.glsl");
  program->addShader(vs);
  program->addShader(fs);

  // rotate cube is the same as apply inverse rotate to textcoord
  osg::Matrixf texMat = osg::Matrix::inverse(getCubeStartRotate());
  auto textureMatrix = new osg::Uniform("textureMatrix", texMat);

  ss->addUniform(textureMatrix);
  ss->addUniform(new osg::Uniform("cubemap", 0));
  ss->setAttribute(program);

  return leaf;
}

int main(int argc, char *argv[])
{
  osgViewer::Viewer viewer;

  auto root = new osg::Group;

  root->addChild(createSkyBox());
  root->addChild(createBall());

  viewer.setSceneData(root);
  viewer.setLightingMode(osg::View::NO_LIGHT);

  return viewer.run();
}
