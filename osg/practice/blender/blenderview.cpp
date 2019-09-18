#include "blenderview.h"
#include "common.h"
#include "blender.h"
#include "prerequisite.h"

namespace zxd {

//------------------------------------------------------------------------------
BlenderViewBase::BlenderViewBase(osg::GraphicsContext* gc) {
  _camera->setCullingMode(osg::CullSettings::NO_CULLING);
  _camera->setGraphicsContext(gc);
  // compute near far cause culling problem during grs operatoin
  _camera->setComputeNearFarMode(osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);

  mHudCamera = zxd::createHudCamera();

  mRoot = new osg::Group();
  mRoot->addChild(mHudCamera);
  setSceneData(mRoot);

  setBorder(true);
}

//------------------------------------------------------------------------------
void BlenderViewBase::setBorder(bool b) {
  if (getBorder() == b) return;

  if (!b) {
    mHudCamera->removeChild(mBorder);
  } else {
    if (!mBorder) {
      // create border and rect
      mBorder = new osg::Geode;
      createBorder();
      mBorder->addDrawable(mGmBorder);
    }
    mHudCamera->addChild(mBorder);
  }
}

//------------------------------------------------------------------------------
void BlenderViewBase::setViewport(float x, float y, float width, float height) {
  _camera->setViewport(x, y, width, height);
  // reset hud camera ortho
  mHudCamera->setProjectionMatrixAsOrtho(
    x, width + x, y, height + y, -50.0f, 50.0f);

  // reset border vertices
  osg::Vec3Array* vertices =
    static_cast<osg::Vec3Array*>(mGmBorder->getVertexArray());

  // TODO fix left border invisible problem
  vertices->at(0) = osg::Vec3(x, y, 0);
  vertices->at(1) = osg::Vec3(x + width, y, 0);
  vertices->at(2) = osg::Vec3(x + width, y + height, 0);
  vertices->at(3) = osg::Vec3(x, y + height, 0);
  mGmBorder->dirtyDisplayList();
  mGmBorder->dirtyBound();
  vertices->dirty();

  doSetViewport(x, y, width, height);
}

//------------------------------------------------------------------------------
void BlenderViewBase::createBorder() {
  mGmBorder = new osg::Geometry;
  osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array();
  osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array();
  colors->setBinding(osg::Array::BIND_OVERALL);

  mGmBorder->setVertexArray(vertices);
  mGmBorder->setColorArray(colors);

  colors->push_back(mBorderColor);

  // just put garbage data here, it will be reset at setViewport
  vertices->push_back(osg::Vec3());
  vertices->push_back(osg::Vec3());
  vertices->push_back(osg::Vec3());
  vertices->push_back(osg::Vec3());

  mGmBorder->addPrimitiveSet(new osg::DrawArrays(GL_LINE_LOOP, 0, 4));

  osg::StateSet* ss = mGmBorder->getOrCreateStateSet();
  ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
  ss->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
}

//------------------------------------------------------------------------------
BlenderView::BlenderView(osg::GraphicsContext* gc, ViewType type)
    : BlenderViewBase(gc), mMiniAxesSize(30.0f), mViewType(type) {
  mText = zxd::createText(
    sgBlender->getFont(), osg::Vec3(), "text", sgBlender->getFontSize());
  osg::ref_ptr<osg::Geode> textLeaf = new osg::Geode();
  textLeaf->addDrawable(mText);

  // create mini axes
  mMiniAxes = new zxd::Axes;
  mMiniAxes->setLabel(true);
  mMiniAxes->setLineWidth(1.5f);

  mMiniAxes->bind(_camera);

  mHudCamera->addChild(mMiniAxes);
  mHudCamera->addChild(textLeaf);
}

//------------------------------------------------------------------------------
void BlenderView::doSetViewport(float x, float y, float width, float height) {
  if (mViewType == VT_USER)
    _camera->setProjectionMatrixAsPerspective(
      zxd::getBestFovy(), width / height, 0.1f, 1000.0f);

  // ortho matrix will be set during toggling quad view
  // else
  //_camera->setProjectionMatrixAsOrtho(
  // x, width + x, y, height + y, -1000.0f, 1000.0f);

  // align ortho with viewport, so you can create things in window coordinate
  // and add to this camera directly. Although you have to translate existing
  // elements
  mHudCamera->setProjectionMatrixAsOrtho(
    x, width + x, y, height + y, -50.0f, 50.0f);
  osg::Vec3 offset(x, y, 0);

  mText->setPosition(
    osg::Vec3(10.0f, height - 10.0f - mText->getCharacterHeight(), 0.0f) +
    offset);
  // place it at low left corner
  mMiniAxes->setMatrix(
    osg::Matrix::scale(mMiniAxesSize, mMiniAxesSize, mMiniAxesSize) *
    osg::Matrix::translate(mMiniAxesSize * 1.35f, mMiniAxesSize + 1.35f, 0) *
    osg::Matrix::translate(osg::Vec3(x, y, 0)));
}

//------------------------------------------------------------------------------
void BlenderView::setGridFloor(osg::ref_ptr<zxd::GridFloor> v) {
  if (mGridFloor && mGridFloor->getNumParents() != 0)
    mRoot->removeChild(mGridFloor);
  mGridFloor = v;
  mRoot->addChild(mGridFloor);
}

//------------------------------------------------------------------------------
TextView::TextView(osg::GraphicsContext* gc) : BlenderViewBase(gc) {
  osgText::Font* font = sgBlender->getFont();
  float fontSize = sgBlender->getFontSize();

  mOpText = zxd::createText(font, osg::Vec3(10.0f, 10.0f, 0.0f), "", fontSize);
  mPivotText =
    zxd::createText(font, osg::Vec3(500.0f, 10.0f, 0.0f), "", fontSize);

  osg::ref_ptr<osg::Geode> textLeaf = new osg::Geode();
  textLeaf->addDrawable(mOpText);
  textLeaf->addDrawable(mPivotText);

  _camera->setClearColor(osg::Vec4(0.25f, 0.25f, 0.25f, 1.0f));
  //_camera->setClearMask(GL_COLOR_BUFFER_BIT);

  mHudCamera->addChild(textLeaf);
}

//------------------------------------------------------------------------------
void TextView::doSetViewport(float x, float y, float width, float height) {
  osg::Vec3 offset(x, y, 0);
  mOpText->setPosition(mOpText->getPosition() + offset);
  mPivotText->setPosition(mPivotText->getPosition() + offset);
}
}
