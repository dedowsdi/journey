#include "blender.h"
#include "common.h"
#include "auxiliary.h"
#include "pivot.h"
#include "blenderobject.h"
#include "gridfloor.h"
#include "objectoperation.h"
#include <osg/ValueObject>
#include <osgDB/ReadFile>
#include <osg/CullFace>
#include <osgViewer/ViewerEventHandlers>
#include "prerequisite.h"

namespace zxd {

//------------------------------------------------------------------------------
Blender::Blender()
    : mOutLineColor(osg::Vec4(1.0f, 0.5f, 0.0f, 1.0f)),
      mFontSize(12.0f),
      mSelectMask(1 << 0),
      mInfoHeight(30.0f),
      mGridLines(16),
      mGridScale(5.0f),
      mGridSubdivisions(4) {}

//------------------------------------------------------------------------------
void Blender::init(osgViewer::CompositeViewer* viewer, osg::Node* node) {

  setViewer(viewer);
  mFont = osgText::readFontFile("fonts/arial.ttf");

  mRoot = new osg::Group();
  osg::ref_ptr<zxd::BlenderObject> bo = new zxd::BlenderObject();
  bo->getModel()->addChild(node);
  mRoot->addChild(bo);

  createViews();

  mCursor = new zxd::Cursor();
  mRoot->addChild(mCursor);

  mPivot = new zxd::Pivot();
  mPivot->setText(mInfoView->getPivotText());
  mPivot->setBlender(this);

  mPivotAxes = new osg::AutoTransform();
  mPivotAxes->setAutoScaleToScreen(true);
  osg::ref_ptr<zxd::Axes> axes = new zxd::Axes();
  axes->setMatrix(osg::Matrix::scale(50.0f, 50.0f, 50.0f));
  mPivotAxes->addChild(axes);

  mPivot->updateText();
}

//------------------------------------------------------------------------------
void Blender::createViews() {

  osg::GraphicsContext* gc = createGraphicsContext();
  createUserView(gc);
  createInfoView(gc);

  /*
   * it's not allowed to add or remove views during frame with CompositeViewer,
   * you can only set camera mask = 0 or set gc to 0
   */
  mFrontView = createOrthoView(gc);
  mRightView = createOrthoView(gc);
  mTopView = createOrthoView(gc);

  //"hide" them
  mFrontView->getCamera()->setNodeMask(0);
  mRightView->getCamera()->setNodeMask(0);
  mTopView->getCamera()->setNodeMask(0);

  // add only user view  and info view by default
  mViewer->addView(mUserView);
  mViewer->addView(mInfoView);
  mViewer->addView(mFrontView);
  mViewer->addView(mRightView);
  mViewer->addView(mTopView);

  const osg::GraphicsContext::Traits* gct = gc->getTraits();

  GLint width = gct->width;
  GLint height = gct->height;
  //GLint x = gct->x;
  //GLint y = gct->y;

  mUserView->setViewport(0, 0 + mInfoHeight, width, height - mInfoHeight);
  mInfoView->setViewport(0, 0, width, mInfoHeight);
}

//------------------------------------------------------------------------------
void Blender::toggleQuadView() {
  // adjust size
  osg::Viewport* vp = mUserView->getCamera()->getViewport();
  GLint x = vp->x();
  GLint y = vp->y();
  GLint width = vp->width();
  GLint height = vp->height();

  if (mFrontView->getCamera()->getNodeMask() != 0) {
    mFrontView->getCamera()->setNodeMask(0);
    mRightView->getCamera()->setNodeMask(0);
    mTopView->getCamera()->setNodeMask(0);

    mUserView->setViewport(x - width, y - height, width * 2, height * 2);

  } else {
    mFrontView->getCamera()->setNodeMask(-1);
    mRightView->getCamera()->setNodeMask(-1);
    mTopView->getCamera()->setNodeMask(-1);

    osg::Matrix orthoProj;

    zxd::BlenderManipulator* camMan =
      static_cast<zxd::BlenderManipulator*>(mUserView->getCameraManipulator());

    double distance = camMan->getDistance();
    const osg::Vec3& center = camMan->getCenter();

    // adjust ortho matrix
    if (!zxd::Math::perspToOrtho(
          mUserView->getCamera()->getProjectionMatrix(), distance, orthoProj))
      OSG_FATAL
        << "failed to convert perspect to ortho during quad view toggling"
        << std::endl;

    mFrontView->getCamera()->setProjectionMatrix(orthoProj);
    mTopView->getCamera()->setProjectionMatrix(orthoProj);
    mRightView->getCamera()->setProjectionMatrix(orthoProj);

    osg::Matrix translate = osg::Matrix::translate(-center);
    // distance doesn't matter for ortho projection
    mFrontView->getCamera()->setViewMatrix(
      translate * osg::Matrix::rotate(-osg::PI_2, osg::X_AXIS));
    {
      camMan = static_cast<zxd::BlenderManipulator*>(
        mFrontView->getCameraManipulator());
      camMan->setCenter(center);
      camMan->setDistance(distance);
      camMan->setRotation(osg::PI_2, 0);
    }
    {
      camMan = static_cast<zxd::BlenderManipulator*>(
        mRightView->getCameraManipulator());
      camMan->setCenter(center);
      camMan->setDistance(distance);
      camMan->setRotation(osg::PI_2, osg::PI_2);
    }
    {
      camMan =
        static_cast<zxd::BlenderManipulator*>(mTopView->getCameraManipulator());
      camMan->setCenter(center);
      camMan->setDistance(distance);
      camMan->setRotation(0, 0);
    }

    // mTopView->getCamera()->setViewMatrix(translate);

    // mRightView->getCamera()->setViewMatrix(
    // translate * osg::Matrix::rotate(-osg::PI_2, osg::X_AXIS) *
    // osg::Matrix::rotate(-osg::PI_2, osg::Y_AXIS));

    GLdouble halfWidth = 0.5f * width;
    GLdouble halfHeight = 0.5f * height;

    mFrontView->setViewport(x, y, halfWidth, halfHeight);
    mRightView->setViewport(x + halfWidth, y, halfWidth, halfHeight);
    mTopView->setViewport(x, y + halfHeight, halfWidth, halfHeight);
    mUserView->setViewport(
      x + halfWidth, y + halfHeight, halfWidth, halfHeight);

    mFrontView->setGridFloor(
      createOrthoGridFloor(osg::X_AXIS, osg::Z_AXIS, mFrontView->getCamera()));
    mRightView->setGridFloor(
      createOrthoGridFloor(osg::Y_AXIS, osg::Z_AXIS, mRightView->getCamera()));
    mTopView->setGridFloor(
      createOrthoGridFloor(osg::X_AXIS, osg::Y_AXIS, mTopView->getCamera()));
  }
}

//-------------------------------------------------Sekurosufia-----------------------------
void Blender::reputPivotAxes() {
  zxd::removeNodeParents(mPivotAxes);

  // only show axes if there exists selected object
  if (!mCurObject) return;

  switch (mPivot->getType()) {
    case zxd::Pivot::PT_ACTIVEELEMENT:
      mCurObject->addChild(mPivotAxes);
      break;
    case zxd::Pivot::PT_3DCURSOR:
      mCursor->addChild(mPivotAxes);
      break;
    default:
      throw new std::runtime_error("undefined pivot type");
  }
}

//------------------------------------------------------------------------------
osg::GraphicsContext* Blender::createGraphicsContext() {
  // create graphics context
  osg::ref_ptr<osg::GraphicsContext::Traits> traits =
    new osg::GraphicsContext::Traits();

  GLuint width = 800, height = 600;
  osg::GraphicsContext::WindowingSystemInterface* wsi =
    osg::GraphicsContext::getWindowingSystemInterface();
  if (!wsi) OSG_FATAL << "failed to get window system interface " << std::endl;
  wsi->getScreenResolution(
    osg::GraphicsContext::ScreenIdentifier(0), width, height);

  traits->x = 0;
  traits->y = 0;
  traits->width = width;
  traits->height = height;
  traits->doubleBuffer = true;
  traits->sharedContext = 0;
  traits->windowDecoration = false;

  osg::GraphicsContext* gc =
    osg::GraphicsContext::createGraphicsContext(traits);
  if (!gc) OSG_FATAL << "failed to creaate graphics context " << std::endl;

  return gc;
}

//------------------------------------------------------------------------------
void Blender::createUserView(osg::GraphicsContext* gc) {
  mUserView = new zxd::BlenderView(gc, zxd::BlenderView::VT_USER);  // main view

  osg::ref_ptr<osgViewer::StatsHandler> sh = new osgViewer::StatsHandler();
  sh->setKeyEventTogglesOnScreenStats(osgGA::GUIEventAdapter::KEY_I);
  // add stats handler and cam man to user view
  mUserView->addEventHandler(sh);

  osg::ref_ptr<zxd::OperationEventHandler> opHandler =
    new zxd::OperationEventHandler();
  opHandler->setView(mUserView);
  mUserView->addEventHandler(opHandler);

  osg::ref_ptr<zxd::GridFloor> gf =
    new zxd::GridFloor(osg::X_AXIS, osg::Y_AXIS);
  gf->setScale(mGridScale);
  gf->setLines(mGridLines);
  gf->setSubDivisions(mGridSubdivisions);
  gf->rebuild();
  gf->setNodeMask(~sgBlender->getSelectMask());
  mUserView->setGridFloor(gf);

  osg::ref_ptr<zxd::BlenderManipulator> camMan = new zxd::BlenderManipulator();
  camMan->setCamera(mUserView->getCamera());
  camMan->setViewText(mUserView->getText());
  camMan->setOrtho(false);
  mUserView->setCameraManipulator(camMan);

  mUserView->getRoot()->addChild(mRoot);
}

//------------------------------------------------------------------------------
void Blender::createInfoView(osg::GraphicsContext* gc) {
  mInfoView = new zxd::TextView(gc);  // operation information
}

//------------------------------------------------------------------------------
zxd::BlenderView* Blender::createOrthoView(osg::GraphicsContext* gc) {
  osg::ref_ptr<zxd::BlenderView> view =
    new zxd::BlenderView(gc, zxd::BlenderView::VT_ORTHO);  // main view

  osg::ref_ptr<zxd::OperationEventHandler> opHandler =
    new zxd::OperationEventHandler();
  opHandler->setView(view);
  view->addEventHandler(opHandler);

  osg::ref_ptr<zxd::BlenderManipulator> camMan = new zxd::BlenderManipulator();
  camMan->setCamera(view->getCamera());
  camMan->setViewText(view->getText());
  view->setCameraManipulator(camMan);
  camMan->setOrtho(true);

  view->getRoot()->addChild(mRoot);

  return view.release();
}

//------------------------------------------------------------------------------
zxd::GridFloor* Blender::createOrthoGridFloor(
  const osg::Vec3& v0, const osg::Vec3& v1, osg::Camera* camera) {
  double left, right, bottom, top, near, far;

  if (!camera->getProjectionMatrixAsOrtho(left, right, bottom, top, near, far))
    OSG_FATAL << "failed to get ortho from camera " << camera << std::endl;

  double width = right - left;
  double height = top - bottom;
  double l = std::max(width, height);

  osg::ref_ptr<zxd::GridFloor> gridfloor = new zxd::GridFloor(v0, v1);
  gridfloor->setScale(mGridScale);
  gridfloor->setSubDivisions(mGridSubdivisions);
  gridfloor->setLines(std::ceil(l / mGridScale));
  gridfloor->rebuild();

  gridfloor->setNodeMask(~getSelectMask());

  return gridfloor.release();
}

//------------------------------------------------------------------------------
inline BlenderObject* OperationEventHandler::getCurObject() {
  return sgBlender->getCurObject();
}

//------------------------------------------------------------------------------
inline ObjectOperation* OperationEventHandler::getCurOperation() {
  return sgBlender->getCurOperation();
}

//------------------------------------------------------------------------------
void OperationEventHandler::setView(zxd::BlenderView* v) {
  mView = v;
  setCamera(mView->getCamera());
}

//------------------------------------------------------------------------------
void OperationEventHandler::clearRotation() {
  // clear orientation
  osg::Vec3 translation;
  osg::Quat rotation;
  osg::Vec3 scale;
  osg::Quat so;
  getCurObject()->getMatrix().decompose(translation, rotation, scale, so);
  getCurObject()->setMatrix(
    osg::Matrix::scale(scale) * osg::Matrix::translate(translation));
}

//------------------------------------------------------------------------------
void OperationEventHandler::clearTranslation() {
  // clear translation
  const osg::Matrix& m = getCurObject()->getMatrix();
  getCurObject()->postMult(osg::Matrix::translate(-m.getTrans()));
}

//------------------------------------------------------------------------------
void OperationEventHandler::clearScale() {
  osg::Vec3 translation;
  osg::Quat rotation;
  osg::Vec3 scale;
  osg::Quat so;
  getCurObject()->getMatrix().decompose(translation, rotation, scale, so);
  getCurObject()->setMatrix(
    osg::Matrix::rotate(rotation) * osg::Matrix::translate(translation));
}

//------------------------------------------------------------------------------
void OperationEventHandler::selectObject(const osgGA::GUIEventAdapter& ea) {
  // select current object
  osgUtil::IntersectionVisitor iv;
  osg::ref_ptr<osgUtil::LineSegmentIntersector> lsi =
    new osgUtil::LineSegmentIntersector(
      osgUtil::Intersector::WINDOW, ea.getX(), ea.getY());
  iv.setIntersector(lsi);
  iv.setTraversalMask(sgBlender->getSelectMask());

  mCamera->accept(iv);

  if (lsi->containsIntersections()) {
    const osgUtil::LineSegmentIntersector::Intersection& result =
      *lsi->getIntersections().begin();

    BlenderObject* curObj = getCurObject();

    for (GLuint i = 0; i < result.nodePath.size(); ++i) {
      osg::Node* node = result.nodePath[i];
      bool isBLenderObject = false;
      if (node->getUserValue("BlenderObject", isBLenderObject)) {
        if (curObj && curObj != node) curObj->deselect();
        curObj = static_cast<zxd::BlenderObject*>(node);
        curObj->select();
        sgBlender->setCurObject(curObj);
        break;
      }
    }

  } else {
    // do nothing if nothing hitten
    // getCurObject()->deselect();
  }
}

//------------------------------------------------------------------------------
void OperationEventHandler::placeCursor(const osgGA::GUIEventAdapter& ea) {
  osg::ref_ptr<osgUtil::LineSegmentIntersector> lsi =
    new osgUtil::LineSegmentIntersector(
      osgUtil::Intersector::WINDOW, ea.getX(), ea.getY());
  osgUtil::IntersectionVisitor iv(lsi);
  iv.setTraversalMask(sgBlender->getSelectMask());
  mCamera->accept(iv);
  osg::ref_ptr<zxd::Cursor> cursor = sgBlender->getCursor();

  if (lsi->containsIntersections()) {
    const osgUtil::LineSegmentIntersector::Intersection& intersection =
      *lsi->getIntersections().begin();

    // get intersection point in view space
    osg::Vec3 pos = intersection.localIntersectionPoint *
                    osg::computeLocalToWorld(intersection.nodePath);
    cursor->setMatrix(osg::Matrix::translate(pos));
  } else {
    const osg::Matrix& matView = mCamera->getViewMatrix();
    // get original cursor position in view space
    osg::Vec3 origPos = cursor->getMatrix().getTrans() * matView;

    // get camera ray intersection with near plane
    osg::Matrix matrix;
    if (mCamera->getViewport())
      matrix.preMult(mCamera->getViewport()->computeWindowMatrix());
    matrix.preMult(mCamera->getProjectionMatrix());
    matrix.invert(matrix);

    // point on near plane in view space
    osg::Vec3 p = osg::Vec3(ea.getX(), ea.getY(), 0) * matrix;

    // scale according z to place p at plane that contain origin cursor
    p *= origPos.z() / p.z();

    // now get final world position
    osg::Matrix matInvView = osg::Matrix::inverse(matView);

    cursor->setMatrix(osg::Matrix::translate(p * matInvView));
  }
}

//------------------------------------------------------------------------------
bool OperationEventHandler::handle(
  const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) {
  switch (ea.getEventType()) {
    case osgGA::GUIEventAdapter::KEYDOWN:
      switch (ea.getUnmodifiedKey()) {
        case osgGA::GUIEventAdapter::KEY_G:
          if (!getCurObject()) break;

          if (ea.getModKeyMask() & osgGA::GUIEventAdapter::MODKEY_ALT) {
            clearTranslation();
          } else {
            // do nothing if it's the same operation type
            zxd::ObjectOperation* curOp = getCurOperation();
            if (curOp && curOp->getType() == "grab") break;

            createNewOperation(ea, new zxd::GrabOperation());
          }
          break;
        case osgGA::GUIEventAdapter::KEY_S:
          if (!getCurObject()) break;

          if (ea.getModKeyMask() & osgGA::GUIEventAdapter::MODKEY_ALT) {
            clearScale();
          } else {
            // do nothing if it's the same operation type
            zxd::ObjectOperation* curOp = getCurOperation();
            if (curOp && curOp->getType() == "scale") break;

            createNewOperation(ea, new zxd::ScaleOperation());
          }
          break;
        case osgGA::GUIEventAdapter::KEY_R:
          if (!getCurObject()) break;

          if (ea.getModKeyMask() & osgGA::GUIEventAdapter::MODKEY_ALT) {
            clearRotation();
          } else {
            // do nothing if it's the same operation type
            zxd::ObjectOperation* curOp = getCurOperation();
            if (curOp && curOp->getType() == "rotate") {
              static_cast<zxd::RotateOperation*>(curOp)->toggleMode();
              break;
            }
            createNewOperation(ea, new zxd::RotateOperation());
          }

          break;
        case osgGA::GUIEventAdapter::KEY_Comma:
          // disallow pivot cange during operation
          if (sgBlender->getCurOperation()) break;
          sgBlender->getPivot()->setType(zxd::Pivot::PT_ACTIVEELEMENT);
          break;
        case osgGA::GUIEventAdapter::KEY_Period:
          if (sgBlender->getCurOperation()) break;
          sgBlender->getPivot()->setType(zxd::Pivot::PT_3DCURSOR);
          break;

        case osgGA::GUIEventAdapter::KEY_Q:

          if (sgBlender->getCurOperation()) break;

          if (ea.getModKeyMask() & osgGA::GUIEventAdapter::MODKEY_CTRL &&
              ea.getModKeyMask() & osgGA::GUIEventAdapter::MODKEY_ALT) {
            sgBlender->toggleQuadView();
          }

          break;

        default:
          break;
      }
      break;
    default:
      break;

    case osgGA::GUIEventAdapter::PUSH:

      if (getCurOperation()) {
        // operation confirm or cancel
        if (ea.getButton() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON) {
          OSG_NOTICE << "confirm operation" << std::endl;
          getCurOperation()->confirm();
        } else {
          getCurOperation()->cancel();
          OSG_NOTICE << "cancel operation" << std::endl;
        }
        mView->removeEventHandler(getCurOperation());
        // clear operation
        sgBlender->clearOperation();

      } else {
        // select object
        if (ea.getButton() == osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON) {
          selectObject(ea);
        } else if (ea.getButton() ==
                   osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON) {
          // re place cursor
          placeCursor(ea);
        }
      }
  }
  return false;  // return true will stop event
}

//------------------------------------------------------------------------------
void OperationEventHandler::createNewOperation(
  const osgGA::GUIEventAdapter& ea, ObjectOperation* op) {
  OSG_NOTICE << "create new operation " << std::endl;
  ObjectOperation* curOp = getCurOperation();

  if (curOp) {
    // do nothing if curreng operation is the same tyep as T
    if (curOp->getType() == op->getType()) return;

    OSG_NOTICE << "reuse last constrainer " << std::endl;
    // create new operation, use current axis constrainer
    osg::ref_ptr<zxd::AxisConstrainer> ac = curOp->getAxisConstrainer();

    curOp->cancel();
    op->setAxisConstrainer(ac);

  } else {
    op->setAxisConstrainer(new zxd::AxisConstrainer());
  }

  op->init(mView, osg::Vec2(ea.getX(), ea.getY()));

  // add axis constrainer to scene
  zxd::AxisConstrainer* ac = op->getAxisConstrainer();
  ac->setNodeMask(-1 & ~sgBlender->getSelectMask());
  sgBlender->getRoot()->addChild(ac);
  sgBlender->setCurOperation(op);
  OSG_NOTICE << "add constrainer to scene" << std::endl;
  osg::Node* handle = op->getHandle();
  if (handle) {
    handle->setNodeMask(-1 & ~sgBlender->getSelectMask());
    mView->getHudCamera()->addChild(handle);
  }
}
}
