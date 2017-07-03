#ifndef BLENDER_H
#define BLENDER_H

#include <osg/Group>
#include <osg/ref_ptr>
#include <osgText/Text>
#include <osgGA/GUIEventHandler>
#include "pivot.h"
#include <osgViewer/Viewer>
#include <osg/AutoTransform>
#include "objectoperation.h"
#include "blenderobject.h"
#include "gridfloor.h"
#include <osgViewer/CompositeViewer>
#include "zmath.h"
#include "blendermanipulator.h"
#include "blenderview.h"
#include <osgViewer/CompositeViewer>
#include <osg/Viewport>

namespace zxd {

class ObjectOperation;
class BlenderObject;
class GridFloor;
class Cursor;
class Pivot;

/*
 * Main scene, also used as application
 */
class Blender : public osg::Referenced {
protected:
  GLfloat mFontSize;
  GLfloat mInfoHeight;
  GLuint mSelectMask;
  osg::Vec4 mOutLineColor;

  GLuint mGridLines;
  GLuint mGridSubdivisions;
  GLfloat mGridScale;

  osg::ref_ptr<zxd::ObjectOperation> mCurOperation;
  osg::ref_ptr<zxd::BlenderObject> mCurObject;
  osg::ref_ptr<zxd::GridFloor> mGridFloor;
  osg::ref_ptr<osgText::Font> mFont;
  osg::ref_ptr<osgText::Text> mViewText;  // view text
  osg::ref_ptr<zxd::Cursor> mCursor;
  osg::ref_ptr<zxd::Pivot> mPivot;
  osg::ref_ptr<osg::AutoTransform> mPivotAxes;

  osg::ref_ptr<zxd::BlenderView> mFrontView;
  osg::ref_ptr<zxd::BlenderView> mRightView;
  osg::ref_ptr<zxd::BlenderView> mTopView;
  osg::ref_ptr<zxd::BlenderView> mUserView;  // main view
  osg::ref_ptr<zxd::TextView> mInfoView;     // operation information

  //do not create viewer inside blender, you will get segment fault during shut
  //down, i have no dear why it happened.
  osgViewer::CompositeViewer* mViewer;

  osg::ref_ptr<osg::Group> mRoot;

public:
  Blender();
  ~Blender(){
    OSG_NOTICE << "blender destructer" << std::endl;
  }

  static osg::ref_ptr<Blender>& instance() {
    static osg::ref_ptr<Blender> blender = new Blender();
    return blender;
  }

  void init(osgViewer::CompositeViewer* viewer, osg::Node* node);
  virtual void createViews();

  void toggleQuadView() ;

  // place axes at corsor or selected object, according to current pivot
  void reputPivotAxes();

  osg::ref_ptr<zxd::ObjectOperation> getCurOperation() const {
    return mCurOperation;
  }
  void setCurOperation(osg::ref_ptr<zxd::ObjectOperation> v) {
    mCurOperation = v;
    reputPivotAxes();
  }

  void clearOperation() {
    mCurOperation = osg::ref_ptr<zxd::ObjectOperation>();
  }

  osg::ref_ptr<zxd::BlenderObject> getCurObject() const { return mCurObject; }
  void setCurObject(osg::ref_ptr<zxd::BlenderObject> v) {
    mCurObject = v;
    reputPivotAxes();
  }

  osg::ref_ptr<zxd::GridFloor> getGridFloor() const { return mGridFloor; }
  void setGridFloor(osg::ref_ptr<zxd::GridFloor> v) { mGridFloor = v; }

  osg::ref_ptr<zxd::Cursor> getCursor() const { return mCursor; }
  void setCursor(osg::ref_ptr<zxd::Cursor> v) { mCursor = v; }

  osg::ref_ptr<zxd::Pivot> getPivot() const { return mPivot; }
  void setPivot(osg::ref_ptr<zxd::Pivot> v) { mPivot = v; }

  osg::Vec4 getOutLineColor() const { return mOutLineColor; }
  void setOutLineColor(osg::Vec4 v) { mOutLineColor = v; }

  GLfloat getFontSize() const { return mFontSize; }
  void setFontSize(GLfloat v) { mFontSize = v; }

  GLuint getSelectMask() const { return mSelectMask; }
  void setSelectMask(GLuint v) { mSelectMask = v; }

  osg::ref_ptr<zxd::BlenderView> getFrontView() const { return mFrontView; }
  void setFrontView(osg::ref_ptr<zxd::BlenderView> v) { mFrontView = v; }

  osg::ref_ptr<zxd::BlenderView> getRightView() const { return mRightView; }
  void setRightView(osg::ref_ptr<zxd::BlenderView> v) { mRightView = v; }

  osg::ref_ptr<zxd::BlenderView> getTopView() const { return mTopView; }
  void setTopView(osg::ref_ptr<zxd::BlenderView> v) { mTopView = v; }

  osg::ref_ptr<zxd::BlenderView> getUserView() const { return mUserView; }
  void setUserView(osg::ref_ptr<zxd::BlenderView> v) { mUserView = v; }

  osg::ref_ptr<zxd::TextView> getInfoView() const { return mInfoView; }
  void setInfoView(osg::ref_ptr<zxd::TextView> v) { mInfoView = v; }

  osgViewer::CompositeViewer* getViewer() const { return mViewer; }
  void setViewer( osgViewer::CompositeViewer* v){mViewer = v;}

  osg::ref_ptr<osg::Group> getRoot() const { return mRoot; }
  void setRoot(osg::ref_ptr<osg::Group> v) { mRoot = v; }

  osg::ref_ptr<osgText::Font> getFont() const { return mFont; }
  void setFont(osg::ref_ptr<osgText::Font> v) { mFont = v; }

  GLuint getGridLines() const { return mGridLines; }
  void setGridLines(GLuint v) { mGridLines = v; }

  GLuint getGridSubdivisions() const { return mGridSubdivisions; }
  void setGridSubdivisions(GLuint v) { mGridSubdivisions = v; }

  GLfloat getGridScale() const { return mGridScale; }
  void setGridScale(GLfloat v) { mGridScale = v; }

protected:
  osg::GraphicsContext* createGraphicsContext();

  void createUserView(osg::GraphicsContext* gc);

  void createInfoView(osg::GraphicsContext* gc);

  zxd::BlenderView* createOrthoView(osg::GraphicsContext* gc);

  /*
   * ortho grid floor must cover the entive projection plane
   */
  zxd::GridFloor* createOrthoGridFloor(
    const osg::Vec3& v0, const osg::Vec3& v1, osg::Camera* camera);
};

/*
 * will be added to specific view
 */
class OperationEventHandler : public osgGA::GUIEventHandler {
protected:
  osg::ref_ptr<zxd::ObjectOperation> operation;
  zxd::BlenderView* mView;
  osg::Camera* mCamera;

public:
  OperationEventHandler() {}

  inline BlenderObject* getCurObject();
  inline ObjectOperation* getCurOperation();

  zxd::BlenderView* getView() const { return mView; }
  void setView(zxd::BlenderView* v);

  osg::Camera* getCamera() const { return mCamera; }
  void setCamera(osg::Camera* v) { mCamera = v; }

protected:
  void clearRotation();
  void clearTranslation();
  void clearScale();

  void selectObject(const osgGA::GUIEventAdapter& ea);

  void placeCursor(const osgGA::GUIEventAdapter& ea);

  virtual bool handle(
    const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);

  void createNewOperation(
    const osgGA::GUIEventAdapter& ea, ObjectOperation* op);
};
;
}

#endif /* BLENDER_H */
