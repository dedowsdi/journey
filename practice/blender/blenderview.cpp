#include "blenderview.h"
#include "common.h"
#include "blender.h"
#include "prerequisite.h"

namespace zxd {
void MiniAxesCallback::operator()(osg::Node* node, osg::NodeVisitor* nv) {
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
BlenderView::BlenderView(osg::GraphicsContext* gc, ViewType type)
    : mMiniAxesSize(30.0f), mViewType(type) {

  _camera->setCullingMode(osg::CullSettings::NO_CULLING);
  _camera->setGraphicsContext(gc);

  //compute near far cause culling problem during grs operatoin
  _camera->setComputeNearFarMode(osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);
  

  mHudCamera = zxd::createHUDCamera();
  // compute near far will ignore text on mini axes
  mHudCamera->setComputeNearFarMode(osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);

  mText = zxd::createText(
    sgBlender->getFont(), osg::Vec3(), "text", sgBlender->getFontSize());
  osg::ref_ptr<osg::Geode> textLeaf = new osg::Geode();
  textLeaf->addDrawable(mText);

  // create mini axes
  mMiniAxes = new zxd::Axes;
  mMiniAxes->setLabel(true);
  mMiniAxes->setLineWidth(1.5f);

  osg::ref_ptr<MiniAxesCallback> cb = new MiniAxesCallback();
  cb->setTargetCamera(_camera);
  mMiniAxes->setUpdateCallback(cb);

  mHudCamera->addChild(mMiniAxes);
  mHudCamera->addChild(textLeaf);

  mRoot = new osg::Group();
  mRoot->addChild(mHudCamera);
  setSceneData(mRoot);
}

//------------------------------------------------------------------------------
void BlenderView::setViewport(float x, float y, float width, float height) {
  _camera->setViewport(x, y, width, height);

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
TextView::TextView(osg::GraphicsContext* gc) {
  osgText::Font* font = sgBlender->getFont();
  float fontSize = sgBlender->getFontSize();

  mOpText = zxd::createText(font, osg::Vec3(10.0f, 10.0f, 0.0f), "", fontSize);
  mPivotText =
    zxd::createText(font, osg::Vec3(500.0f, 10.0f, 0.0f), "", fontSize);

  osg::ref_ptr<osg::Geode> textLeaf = new osg::Geode();
  textLeaf->addDrawable(mOpText);
  textLeaf->addDrawable(mPivotText);

  // mHudCamera = zxd::createHUDCamera();
  // mHudCamera->addChild(textLeaf);

  _camera->setClearColor(osg::Vec4(0.25f, 0.25f, 0.25f, 1.0f));
  _camera->setGraphicsContext(gc);
  //_camera->setClearMask(GL_COLOR_BUFFER_BIT);

  mRoot = new osg::Group();
  // mRoot->addChild(mHudCamera);
  mRoot->addChild(textLeaf);
  setSceneData(mRoot);
}

//------------------------------------------------------------------------------
void TextView::setViewport(float x, float y, float width, float height) {
  _camera->setViewport(x, y, width, height);
  _camera->setViewMatrix(osg::Matrix::identity());
  _camera->setAllowEventFocus(false);  // without this, ortho will no work ?
  //_camera->setProjectionMatrixAsPerspective(
  // zxd::getBestFovy(), width / height, 0.1f, 1000.0f);
  _camera->setProjectionMatrixAsOrtho2D(x, width + x, y, height + y);
  _camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
  // mHudCamera->setProjectionMatrixAsOrtho(
  // x, width + x, y, height + y, -50.0f, 50.0f);

  osg::Vec3 offset(x, y, 0);
  mOpText->setPosition(mOpText->getPosition() + offset);
  mPivotText->setPosition(mPivotText->getPosition() + offset);
}
}
