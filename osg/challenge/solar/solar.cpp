#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osg/MatrixTransform>
#include <osg/ShapeDrawable>
#include <osgDB/ReadFile>
#include <osg/Material>
#include "glm.h"

#define WIDTH 800
#define HEIGHT 800
#define SPEED 1.2
#define NUM_ORBITS 5

class Planet;
std::string textureFiles[6] = {
  "sunmap.jpg", "mercurymap.jpg", "venusmap.jpg", "earthmap.jpg", "marsmap.jpg", "jupitermap.jpg"};
osg::ref_ptr<osg::Group> root;
osg::ref_ptr<Planet> sun;
osg::ref_ptr<osg::ShapeDrawable> geometry;

class Planet : public osg::MatrixTransform
{
protected:
  std::vector<osg::ref_ptr<Planet>> mOrbits;
  osg::ref_ptr<osg::Geode> mLeaf;
  osg::Vec3 mAxis; // rotatoin axix
  osg::Vec3 mUnitPos;
  GLfloat mAngle;  // rotating angle
  GLfloat mDistance; // distance from mother
  GLfloat mRadius;
  GLfloat mSpeed;
  GLuint mTexture;

public:

  Planet(GLfloat radius = 1, GLfloat distance = 0):
    mRadius(radius),
    mDistance(distance)
  {
    mAngle = glm::linearRand(0.0f, glm::f2pi);
    mSpeed = glm::linearRand(0.0, SPEED);
    mUnitPos = glm::sphericalRand(1.0); // raondom start unit positoin
    // rotation axis should perp to unit pos
    osg::Vec3 rv = glm::sphericalRand(1.0); // random vector that's not parallel to start unit position
    while(std::abs(rv*mUnitPos) > 0.99)
      rv = glm::sphericalRand(1.0);

    mAxis = rv^mUnitPos;
    mAxis.normalize();
    //mTexture = textures[glm::linearRand(1, 5)];
    updateMatrix();
    
    osg::ref_ptr<osg::MatrixTransform> mt  = new osg::MatrixTransform();
    mt->setMatrix(osg::Matrix::scale(osg::Vec3(mRadius, mRadius, mRadius)));
    mLeaf = new osg::Geode();
    mLeaf->addDrawable(geometry);
    mt->addChild(mLeaf);
    addChild(mt);

    osg::StateSet* ss = getOrCreateStateSet();
    osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D();
    texture->setImage(
        osgDB::readImageFile("Images/" + textureFiles[static_cast<int>(glm::linearRand(1,6))]));
    ss->setTextureAttributeAndModes(0, texture);
  }

  void update(GLfloat dt)
  {
    mAngle += dt * mSpeed;
    updateMatrix();
    for(auto iter = mOrbits.begin(); iter != mOrbits.end(); ++iter){
      (*iter)->update(dt);
    }
  }

  void updateMatrix()
  {
    _matrix = osg::Matrix::rotate(mAngle, mAxis);
    _matrix.preMultTranslate(mUnitPos * mDistance);
  };


  //void draw(material& mtl, const mat4& parentM_mat = mat4(1))
  //{
    //glBindTexture(GLtEXTURE2D, mTexture);
    //prg.texUnit = 0;
    //mat4 m = parentM_mat * mMat();
    //prg.updateModel(m);
    //prg.updateLightingUniforms(lights, lm, mtl);

    //geometry.draw();
    //for(auto iter = mOrbits.begin(); iter != mOrbits.end(); ++iter){
      //iter->draw(orbitMtl, parentM_mat * mMatNoScale());
    //}
  //}

  GLfloat speed() const { return mSpeed; }
  void speed(GLfloat v){ mSpeed = v; }

  GLuint texture() const { return mTexture; }
  void texture(GLuint v){ mTexture = v; }

  osg::ref_ptr<osg::Geode> getLeaf() const { return mLeaf; }

  void spawnOrbits(GLint level)
  {
    if(level == 0)
      return;

    --level;
    GLint numOrbits = glm::linearRand(1, NUM_ORBITS);
    while(numOrbits--)
    {
      GLfloat r = mRadius * 0.5;
      GLfloat d = mRadius + r;
      d += glm::linearRand(d*2, d * 6) * std::pow(0.9, level) ;
      mOrbits.push_back(new Planet(r, d));
      addChild(mOrbits.back());
      mOrbits.back()->spawnOrbits(level);
    }
  }
};

class RootUpdate : public osg::NodeCallback
{
  virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
  {
    static GLfloat lastTime = nv->getFrameStamp()->getReferenceTime();
    GLfloat currentTime = nv->getFrameStamp()->getReferenceTime();
    GLfloat deltaTime = currentTime - lastTime;
    lastTime = currentTime;
    sun->update(deltaTime);

    traverse(node,nv);
  }
};

int main(int argc, char* argv[])
{
  osg::ref_ptr<osg::Sphere> shape = new osg::Sphere;
  osg::ref_ptr<osg::TessellationHints> hints = new osg::TessellationHints();
  hints->setDetailRatio(2);
  hints->setCreateNormals(true);
  hints->setCreateTextureCoords(true);
  geometry = new osg::ShapeDrawable(shape, hints);

  sun = new Planet();
  sun->spawnOrbits(3);
  sun->speed(0);

  osg::ref_ptr<osg::LightSource> ls  = new osg::LightSource();
  osg::Light* light = ls->getLight();
  light->setDiffuse(osg::Vec4(1, 1, 1, 1));
  light->setSpecular(osg::Vec4(1, 1, 1, 1));
  light->setPosition(osg::Vec4(0, 0, 0, 1));

  sun->addChild(ls);

  osg::StateSet* ss = sun->getLeaf()->getOrCreateStateSet();

  osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D();
  texture->setImage( osgDB::readImageFile("Images/sunmap.jpg"));
  ss->setTextureAttributeAndModes(0, texture);

  osg::ref_ptr<osg::Material> mtl  = new osg::Material();
  mtl->setEmission(osg::Material::FRONT_AND_BACK, osg::Vec4(1, 1, 1, 1));
  ss->setAttribute(mtl);

  
  sun->setInitialBound(osg::BoundingSphere(osg::Vec3(), 10));

  root = new osg::Group;
  root->addChild(sun);
  root->addUpdateCallback(new RootUpdate());

  osgViewer::Viewer viewer;
  viewer.addEventHandler(new osgViewer::StatsHandler);
  viewer.setSceneData(root);
  return viewer.run();
}
