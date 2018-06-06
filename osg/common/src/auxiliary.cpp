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
#include <osgText/Text>
#include "zmath.h"

namespace zxd {

//------------------------------------------------------------------------------
CameraViewCallback::CameraViewCallback(
  osg::Camera* camera, const osg::Matrix& toViewMat)
    : mCamera(camera), mToViweMat(toViewMat) {
  const osg::Matrix& fromViweMat = mCamera->getViewMatrix();
  osg::Vec3 scale;
  osg::Quat so;

  fromViweMat.decompose(mFromPos, mFromQuat, scale, so);
  toViewMat.decompose(mToPos, mToQuat, scale, so);

  mMotion = new osgAnimation::InOutCubicMotion();
}

void CameraViewCallback::operator()(osg::Node* node, osg::NodeVisitor* nv) {
  static GLfloat lt = nv->getFrameStamp()->getReferenceTime();
  GLfloat t = nv->getFrameStamp()->getReferenceTime();
  GLfloat dt = t - lt;

  mMotion->update(dt);

  if (mMotion->getTime() >= 1.0f) {
    mCamera->setViewMatrix(mToViweMat);
    mCamera->removeUpdateCallback(this);
  } else {
    osg::Quat q;
    osg::Vec3 v;
    GLfloat i = mMotion->getValue();
    q.slerp(i, mFromQuat, mToQuat);
    v = mFromPos * (1 - i) + mToPos * i;
    if (mCamMan) {
      mCamMan->setRotation(q);
      mCamMan->setDistance(v.length());
    } else {
      mCamera->setViewMatrix(
        osg::Matrix::rotate(q) * osg::Matrix::translate(v));
    }
  }
  traverse(node, nv);
}

//--------------------------------------------------------------------
void Axes::AxesCallback::operator()(osg::Node* node, osg::NodeVisitor* nv)
{
  osg::MatrixTransform* mt = node->asTransform()->asMatrixTransform();
  const osg::Matrix& targetView = mTargetCamera->getViewMatrix();

  osg::Matrix m = mt->getMatrix();
  osg::Vec3 translation;
  osg::Quat rotation;
  osg::Vec3 scale;
  osg::Quat so;
  m.decompose(translation, rotation, scale, so);

  // copy rotation, reserve translation and scale
  m = osg::Matrix::scale(scale) * zxd::Math::getMatR(targetView) *
      osg::Matrix::translate(translation);
  mt->setMatrix(m);

  traverse(node, nv);
}

//------------------------------------------------------------------------------
Axes::Axes() {
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
    //mAxes->setUseDisplayList(false);
  }

  leaf->addDrawable(mAxes);
  addChild(leaf);

  osg::StateSet* ss = leaf->getOrCreateStateSet();
  ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
  ss->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
  ss->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
}

//------------------------------------------------------------------------------
void Axes::setLabel(bool b) {
  if (b) {
    if (!mLabelX) {
      // create leabels
      mLabelX = createLabel(osg::X_AXIS, "X");
      mLabelY = createLabel(osg::Y_AXIS, "Y");
      mLabelZ = createLabel(osg::Z_AXIS, "Z");
    }

    if (mLabelX->getNumParents() != 0) return;

    addChild(mLabelX);
    addChild(mLabelY);
    addChild(mLabelZ);
  } else {
    if (!mLabelX) return;

    mLabelX->getParent(0)->removeChild(mLabelX);
    mLabelY->getParent(0)->removeChild(mLabelY);
    mLabelZ->getParent(0)->removeChild(mLabelZ);
  }
}

//------------------------------------------------------------------------------
void Axes::setLineWidth(GLfloat w) {
  osg::StateSet* ss = mAxes->getOrCreateStateSet();
  ss->setAttributeAndModes(new osg::LineWidth(w));
}

//------------------------------------------------------------------------------
GLfloat Axes::getLineWidth() {
  osg::StateSet* ss = mAxes->getOrCreateStateSet();
  osg::LineWidth* lw = static_cast<osg::LineWidth*>(
    ss->getAttribute(osg::StateAttribute::LINEWIDTH));
  return lw ? lw->getWidth() : 1.0f;
}

//--------------------------------------------------------------------
void Axes::bindInverseView(osg::Camera* camera)
{
  if (!mCallback.valid()){
    mCallback = new Axes::AxesCallback();
    addUpdateCallback(mCallback);
  }

  mCallback->setTargetCamera(camera);
}

//--------------------------------------------------------------------
void Axes::unbindInverseView()
{
  removeUpdateCallback(mCallback);
}

//------------------------------------------------------------------------------
osg::ref_ptr<osg::AutoTransform> Axes::createLabel(
  const osg::Vec3& v, const std::string& label) {
  static osg::ref_ptr<osgText::Font> font = osgText::readFontFile("arial.ttf");

  osg::ref_ptr<osg::AutoTransform> at = new osg::AutoTransform();
  at->setPosition(v);
  at->setAutoRotateMode(osg::AutoTransform::ROTATE_TO_SCREEN);

  osg::ref_ptr<osgText::Text> text =
    zxd::createText(font, osg::Vec3(), label, 0.35f);
  text->setColor(osg::Vec4(v, 1.0f));
  // text->setAutoRotateToScreen(true);

  osg::ref_ptr<osg::Geode> leaf = new osg::Geode();
  leaf->addDrawable(text);

  at->addChild(leaf);
  return at;
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

  osg::StateSet* ss = mCursor->getOrCreateStateSet();
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
  mCursor->setInitialBound(osg::BoundingBox(-5, -5, -5, 5, 5, 5));
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
  mGeometry = new osg::Geometry;

  osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array();
  osg::ref_ptr<osg::Vec3Array> colors = new osg::Vec3Array();
  colors->setBinding(osg::Array::BIND_OVERALL);

  mGeometry->setVertexArray(vertices);
  mGeometry->setColorArray(colors);

  // FLT_MAX cause trouble, don't know y
  vertices->push_back(-v * 0.5f * 1000000);
  vertices->push_back(v * 0.5f * 1000000);
  colors->push_back(color);

  mGeometry->addPrimitiveSet(new osg::DrawArrays(GL_LINES, 0, 2));
  osg::StateSet* ss = mGeometry->getOrCreateStateSet();
  ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
  ss->setAttributeAndModes(new osg::LineWidth(1.5f));

  osg::ref_ptr<osg::Geode> leaf = new osg::Geode();
  leaf->addDrawable(mGeometry);
  addChild(leaf);
}

//------------------------------------------------------------------------------
LineSegmentNode::LineSegmentNode() {
  mGeometry = new osg::Geometry;
  osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array();
  osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array();
  colors->setBinding(osg::Array::BIND_OVERALL);

  vertices->push_back(osg::Vec3());
  vertices->push_back(osg::X_AXIS);

  colors->push_back(osg::Vec4(0.0f, 0.0f, 0.0f, 1.0f));

  mGeometry->setVertexArray(vertices);
  mGeometry->setColorArray(colors);

  mGeometry->addPrimitiveSet(new osg::DrawArrays(GL_LINES, 0, 2));

  osg::StateSet* ss = mGeometry->getOrCreateStateSet();
  ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

  osg::ref_ptr<osg::Geode> leaf = new osg::Geode();
  leaf->addDrawable(mGeometry);
  addChild(leaf);
}

//------------------------------------------------------------------------------
void LineSegmentNode::setPosition(const osg::Vec3& v0, const osg::Vec3& v1) {
  setStartPosition(v0);
  setEndPosition(v1);
}

//------------------------------------------------------------------------------
void LineSegmentNode::setStartPosition(const osg::Vec3& v) {
  osg::Vec3Array* vertices =
    static_cast<osg::Vec3Array*>(mGeometry->getVertexArray());
  vertices->at(0) = v;
  vertices->dirty();
  mGeometry->dirtyBound();
  mGeometry->dirtyDisplayList();
}

//------------------------------------------------------------------------------
void LineSegmentNode::setEndPosition(const osg::Vec3& v) {
  osg::Vec3Array* vertices =
    static_cast<osg::Vec3Array*>(mGeometry->getVertexArray());
  vertices->at(1) = v;
  vertices->dirty();
  mGeometry->dirtyBound();
  mGeometry->dirtyDisplayList();
}

//------------------------------------------------------------------------------
DirectionArrow::DirectionArrow(GLfloat size /*= 32.0f*/)
    : mSize(size), mOffset(12.0f) {
  osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D();
  texture->setImage(osgDB::readImageFile("Images/arrow.png"));
  mGeometry =
    osg::createTexturedQuadGeometry(osg::Vec3(-mSize * 0.5f, -mSize * 0.5f, 0),
      osg::X_AXIS * mSize, osg::Y_AXIS * mSize);
  texture->setFilter(
    osg::Texture::MIN_FILTER, osg::Texture::NEAREST_MIPMAP_LINEAR);
  texture->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);

  osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array();
  colors->setBinding(osg::Array::BIND_OVERALL);
  colors->push_back(osg::Vec4(0.0f, 0.0f, 0.0f, 1.0f));
  mGeometry->setColorArray(colors);

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

//------------------------------------------------------------------------------
void DirectionArrow::setColor(const osg::Vec4& color) {
  osg::Vec4Array* colors =
    static_cast<osg::Vec4Array*>(mGeometry->getColorArray());
  (*colors)[0] = color;
  mGeometry->dirtyDisplayList();
}
}
