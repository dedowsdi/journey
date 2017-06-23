#ifndef BLENDER_H
#define BLENDER_H

#include <osg/Group>
#include <osg/ref_ptr>
#include <osgText/Text>
#include <osgGA/GUIEventHandler>
#include "pivot.h"
#include <osgViewer/Viewer>
#include "objectoperation.h"
#include "blenderobject.h"
#include "gridfloor.h"

namespace zxd {

class ObjectOperation;
class BlenderObject;
class GridFloor;
class Cursor;
class Pivot;

class Blender : public osg::Group {
protected:
  osg::ref_ptr<zxd::ObjectOperation> mCurOperation;
  osg::ref_ptr<zxd::BlenderObject> mCurObject;
  osg::ref_ptr<zxd::GridFloor> mGridFloor;
  osg::ref_ptr<osgText::Font> mFont;
  osg::ref_ptr<osgText::Text> mOpText;  // operation text
  osg::ref_ptr<osgText::Text> mPivotText;
  osg::ref_ptr<zxd::Cursor> mCursor;
  osg::ref_ptr<zxd::Pivot> mPivot;
  osg::ref_ptr<zxd::Axes> mPivotAxes;  // always global, not so useful right now
  osg::Vec4 mOutLineColor;
  osg::Camera* mCamera;
  osg::Camera* mHudCamera;
  osgViewer::Viewer* mViewer;
  GLfloat mFontSize;
  GLuint mSelectMask;

public:
  Blender();

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

  osg::Camera* getCamera() const { return mCamera; }
  void setCamera(osg::Camera* v);

  osg::Camera* getHudCamera() const { return mHudCamera; }
  void setHudCamera(osg::Camera* v) { mHudCamera = v; }

  GLfloat getFontSize() const { return mFontSize; }
  void setFontSize(GLfloat v) { mFontSize = v; }

  GLuint getSelectMask() const { return mSelectMask; }
  void setSelectMask(GLuint v) { mSelectMask = v; }

  osgViewer::Viewer* getViewer() const { return mViewer; }
  void setViewer(osgViewer::Viewer* v) { mViewer = v; }

  osg::ref_ptr<osgText::Text> getOpText() const { return mOpText; }
};

class BlendGuiEventhandler : public osgGA::GUIEventHandler {
protected:
  osg::ref_ptr<zxd::ObjectOperation> operation;
  Blender* mBlender;

public:
  BlendGuiEventhandler() : mBlender(0) {}
  Blender* getBlender() const { return mBlender; }
  void setBlender(Blender* v) { mBlender = v; }

  inline BlenderObject* getCurObject() { return mBlender->getCurObject(); }
  inline ObjectOperation* getCurOperation() {
    return mBlender->getCurOperation();
  }

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
