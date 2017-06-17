#include "auxiliary.h"
#include <osg/Point>
#include <osg/Depth>
#include <osg/PointSprite>
#include <osg/Texture2D>
#include <osgDB/ReadFile>
#include <osg/BlendFunc>
#include <osg/LineStipple>
#include <osg/Material>
#include <osg/LineWidth>
#include <osg/TexMat>
#include "common.h"

namespace zxd {

//------------------------------------------------------------------------------
Axes::Axes(osg::Camera* camera /*= 0*/) : mAxisSize(30.0f) {
  osg::ref_ptr<osg::Geode> leaf = new osg::Geode();
  {
    // create axes
    mAxes = new osg::Geometry;

    osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array();
    osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array();

    vertices->push_back(osg::Vec3());
    vertices->push_back(osg::X_AXIS);
    vertices->push_back(osg::Vec3());
    vertices->push_back(osg::Y_AXIS);
    vertices->push_back(osg::Vec3());
    vertices->push_back(osg::Z_AXIS);

    colors->push_back(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));
    colors->push_back(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));
    colors->push_back(osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f));
    colors->push_back(osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f));
    colors->push_back(osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f));
    colors->push_back(osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f));

    mAxes->setVertexArray(vertices);
    mAxes->setColorArray(colors);
    colors->setBinding(osg::Array::BIND_PER_VERTEX);

    mAxes->addPrimitiveSet(new osg::DrawArrays(GL_LINES, 0, 6));
    // mAxes->setDrawCallback(new FixDistanceDrawCalback(3));
    mAxes->setUseDisplayList(false);
  }

  leaf->addDrawable(mAxes);

  addChild(leaf);

  osg::StateSet* ss = getOrCreateStateSet();
  ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
  ss->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
  ss->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
  ss->setAttributeAndModes(new osg::Depth(osg::Depth::LESS, 0.0f, 1.0f, false));
  mFixSizeCallback = new FixSizeCallback(10.0f);
  mFixSizeCallback->setCamera(camera);

  this->setUpdateCallback(mFixSizeCallback);
}

//------------------------------------------------------------------------------
Cursor::Cursor() {
  // point sprite cursor

  mCursor = new osg::Geometry;

  osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array();
  osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array();

  // create arrow
  vertices->push_back(osg::Vec3());
  vertices->push_back(osg::X_AXIS);
  vertices->push_back(osg::Z_AXIS);

  colors->push_back(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));

  mCursor->setVertexArray(vertices);
  mCursor->setColorArray(colors);
  mCursor->addPrimitiveSet(new osg::DrawArrays(GL_POINTS, 0, 1));
  colors->setBinding(osg::Array::BIND_OVERALL);

  osg::StateSet* ss = getOrCreateStateSet();
  ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
  ss->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
  ss->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
  ss->setTextureAttributeAndModes(0, new osg::PointSprite());
  ss->setAttributeAndModes(new osg::Point(32.0f));

  // it might be redundancy to create blendFunc
  osg::ref_ptr<osg::BlendFunc> blendFunc = new osg::BlendFunc();
  blendFunc->setFunction(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  ss->setAttributeAndModes(blendFunc);

  osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D();
  texture->setImage(osgDB::readImageFile("Images/cursor.png"));
  ss->setTextureAttributeAndModes(0, texture);

  osg::ref_ptr<osg::Geode> leaf = new osg::Geode();
  addChild(leaf);
  leaf->addDrawable(mCursor);
}

//------------------------------------------------------------------------------
LightIcon::LightIcon() {
  {
    mLight = new osg::Geometry();
    osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array();
    osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array();
    mLight->setVertexArray(vertices);
    mLight->setColorArray(colors);

    vertices->push_back(osg::Vec3());
    colors->push_back(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));

    mLight->addPrimitiveSet(new osg::DrawArrays(GL_POINTS, 0, 1));
    osg::StateSet* ss = mLight->getOrCreateStateSet();
    ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
    ss->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
    ss->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

    osg::ref_ptr<osg::Material> material = new osg::Material();
    material->setDiffuse(
      osg::Material::FRONT_AND_BACK, osg::Vec4(0.0f, 0.0f, 0.0f, 1.0f));

    ss->setAttributeAndModes(material);

    osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D();
    texture->setImage(osgDB::readImageFile("Images/light.png"));
    texture->setFilter(
      osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR);
    texture->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
    ss->setTextureAttributeAndModes(0, texture);
    ss->setTextureAttributeAndModes(0, new osg::PointSprite());
    ss->setAttributeAndModes(new osg::Point(32.0f));

    osg::ref_ptr<osg::BlendFunc> blendFunc = new osg::BlendFunc();
    blendFunc->setFunction(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    ss->setAttributeAndModes(blendFunc);
  }

  {
    mVerticalLine = new osg::Geometry;
    osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array();
    mVerticalLine->setVertexArray(vertices);

    vertices->push_back(osg::Vec3());
    vertices->push_back(-osg::Z_AXIS);

    mVerticalLine->addPrimitiveSet(new osg::DrawArrays(GL_LINES, 0, 2));
    osg::StateSet* ss = mVerticalLine->getOrCreateStateSet();
    ss->setAttributeAndModes(new osg::LineStipple(2, 0x0f0f));

    mTfLine = new osg::MatrixTransform();
    osg::ref_ptr<osg::Geode> leaf = new osg::Geode();
    leaf->addDrawable(mVerticalLine);
    mTfLine->addChild(leaf);
  }

  osg::ref_ptr<osg::Geode> leaf = new osg::Geode();
  leaf->addDrawable(mLight);
  addChild(leaf);
  addChild(mTfLine);

  setUpdateCallback(new LightIconUpdateCallback);
}

//------------------------------------------------------------------------------
CameraIcon::CameraIcon(osg::Camera* camera) : mCamera(camera) {}

//------------------------------------------------------------------------------
Dot::Dot(GLfloat size /*= 5.0f*/,
  const osg::Vec4 color /*= osg::Vec4(0.0f, 1.0f, 0.5f, 1.0f)*/)
    : mSize(size), mColor(color) {
  mDot = new osg::Geometry;
  osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array();
  mDot->setVertexArray(vertices);

  vertices->push_back(osg::Vec3());

  mDot->addPrimitiveSet(new osg::DrawArrays(GL_POINTS, 0, 1));

  osg::StateSet* ss = mDot->getOrCreateStateSet();
  ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
  ss->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
  ss->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

  setSize(mSize);
  setColor(mColor);

  addChild(mDot);
}

//------------------------------------------------------------------------------
void Dot::setSize(GLfloat v) {
  mSize = v;

  osg::StateSet* ss = mDot->getOrCreateStateSet();
  osg::Point* point =
    static_cast<osg::Point*>(ss->getAttribute(osg::StateAttribute::POINT));
  if (point)
    point->setSize(mSize);
  else
    ss->setAttributeAndModes(new osg::Point(mSize));
}

//------------------------------------------------------------------------------
void Dot::setColor(const osg::Vec4& v) {
  mColor = v;

  osg::StateSet* ss = mDot->getOrCreateStateSet();
  osg::Material* material = static_cast<osg::Material*>(
    ss->getAttribute(osg::StateAttribute::MATERIAL));
  if (material)
    material->setDiffuse(osg::Material::FRONT_AND_BACK, mColor);
  else {
    osg::ref_ptr<osg::Material> material = new osg::Material();
    material->setDiffuse(osg::Material::FRONT_AND_BACK, mColor);
    ss->setAttributeAndModes(material);
  }
}

//------------------------------------------------------------------------------
InfiniteLine::InfiniteLine(const osg::Vec3& v /*= osg::X_AXIS*/,
  const osg::Vec3& color /*= osg::X_AXIS*/) {
  mInfiniteLine = new osg::Geometry;
  osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array();
  osg::ref_ptr<osg::Vec3Array> colors = new osg::Vec3Array();
  colors->setBinding(osg::Array::BIND_OVERALL);

  mInfiniteLine->setVertexArray(vertices);
  mInfiniteLine->setColorArray(colors);

  vertices->push_back(-v * 0.5f * FLT_MAX);
  vertices->push_back(v * 0.5f * FLT_MAX);

  mInfiniteLine->addPrimitiveSet(new osg::DrawArrays(GL_LINES, 0, 2));
  osg::StateSet* ss = mInfiniteLine->getOrCreateStateSet();
  ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
  ss->setAttributeAndModes(new osg::LineWidth(1.5f));

  osg::ref_ptr<osg::Geode> leaf = new osg::Geode();
  leaf->addDrawable(mInfiniteLine);
  addChild(leaf);
}

//------------------------------------------------------------------------------
DirectionArrow::DirectionArrow(GLfloat size /*= 32.0f*/) : mSize(size), mOffset(15.0f) {
   osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D();
   texture->setImage(osgDB::readImageFile("Images/arrow.png"));

  // mGeometry = new osg::Geometry;
  // osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array();
  // osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array();
  // colors->setBinding(osg::Array::BIND_OVERALL);

  // vertices->push_back(osg::Vec3());
  // colors->push_back(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));

  // mGeometry->setVertexArray(vertices);
  // mGeometry->setColorArray(colors);
  // mGeometry->addPrimitiveSet(new osg::DrawArrays(GL_POINTS, 0, 1));

  // osg::ref_ptr<osg::Geode> leaf = new osg::Geode();
  // leaf->addDrawable(mGeometry);

  // const char* vsSource =
  //" #version 120                                          \n"
  //"                                                       \n "
  //" void main()                                           \n "
  //" {                                                     \n "
  //"   gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;  \n "
  //" }                                                     \n ";

  // const char* fsSource =
  //" #version 120                                          \n"
  //" uniform mat4 texMat;                                  \n "
  //" uniform sampler2D tex;                                \n "
  //"                                                       \n "
  //" void main()                                           \n "
  //" {                                                     \n "
  //"   vec4 texCoord = texMat * vec4(gl_PointCoord, 0, 1); \n "
  //"   gl_FragColor = texture2D(tex, texCoord.xy);         \n "
  //" }                                                     \n ";

  //// need shader to rotate point sprite
  // osg::ref_ptr<osg::Program> program = new osg::Program();
  // osg::ref_ptr<osg::Shader> fs =
  // new osg::Shader(osg::Shader::FRAGMENT, fsSource);
  // osg::ref_ptr<osg::Shader> vs = new osg::Shader(osg::Shader::VERTEX,
  // vsSource);
  // program->addShader(vs);
  // program->addShader(fs);

  // leaf->getOrCreateStateSet()->setAttributeAndModes(program);

  mGeometry =
    osg::createTexturedQuadGeometry(osg::Vec3(-mSize * 0.5f, -mSize * 0.5f, 0),
      osg::X_AXIS * mSize, osg::Y_AXIS * mSize);
  osg::ref_ptr<osg::Geode> leaf = new osg::Geode();
  leaf->addDrawable(mGeometry);

  mTransform0 = new osg::MatrixTransform();
  mTransform1 = new osg::MatrixTransform();

  mTransform0->addChild(leaf);
  mTransform1->addChild(leaf);

  addChild(mTransform0);
  addChild(mTransform1);

  setDirection(0, osg::Vec2(0.0f, 1.0f));
  setDirection(1, osg::Vec2(0.0f, -1.0f));

  osg::StateSet* ss = leaf->getOrCreateStateSet();
  ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
  ss->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
  ss->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
  //ss->setAttributeAndModes(new osg::Point(mSize));
  //osg::ref_ptr<osg::PointSprite> ps = new osg::PointSprite();
  //ps->setCoordOriginMode(osg::PointSprite::LOWER_LEFT);
  //ss->setTextureAttributeAndModes(0, ps);
  ss->setTextureAttributeAndModes(0, texture);
}

//------------------------------------------------------------------------------
void DirectionArrow::setDirection(GLuint index, osg::Vec2 d) {
  osg::ref_ptr<osg::MatrixTransform> tf =
    index == 0 ? mTransform0 : mTransform1;

  GLfloat theta = atan2(d.y(), d.x());
  theta -= osg::PI_2;

  tf->setMatrix(osg::Matrix::translate(0, mOffset, 0) *
                osg::Matrix::rotate(theta, osg::Z_AXIS));

  // osg::Vec3 trans(0.5f, 0.5f, 0);
  // osg::Matrix texMat = osg::Matrix::translate(-trans) *
  // osg::Matrix::rotate(-theta, osg::Z_AXIS) *
  // osg::Matrix::translate(trans);

  ////texMat = zxd::transpose(texMat);
  // osg::StateSet* ss = tf->getOrCreateStateSet();
  // ss->getOrCreateUniform("texMat", osg::Uniform::FLOAT_MAT4)->set(texMat);
}
}
