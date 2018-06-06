#ifndef AUXILIARY_H
#define AUXILIARY_H
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/State>
#include <osg/MatrixTransform>
#include <osgGA/GUIEventHandler>
#include <osgUtil/LineSegmentIntersector>
#include <osg/io_utils>
#include <osg/AutoTransform>
#include <osgAnimation/EaseMotion>
#include <osgGA/OrbitManipulator>

namespace zxd {

class MyDrawableCullCallback : public osg::Drawable::CullCallback {
  virtual bool cull(
    osg::NodeVisitor* nv, osg::Drawable* drawable, osg::State* state) const {
    return false;
  }
};

/*
 * rotate and translate camera to new position
 */
class CameraViewCallback : public osg::NodeCallback {
protected:
  osg::ref_ptr<osgAnimation::InOutCubicMotion> mMotion;
  osg::Matrix mToViweMat;
  osg::Quat mFromQuat, mToQuat;
  osg::Vec3 mFromPos, mToPos;
  osg::Camera* mCamera;
  osg::ref_ptr<osgGA::OrbitManipulator> mCamMan;

public:
  CameraViewCallback(osg::Camera* camera, const osg::Matrix& toViewMat);

  virtual void operator()(osg::Node* node, osg::NodeVisitor* nv);

  osg::ref_ptr<osgGA::OrbitManipulator> getCamMan() const { return mCamMan; }
  void setCamMan(osg::ref_ptr<osgGA::OrbitManipulator> v) { mCamMan = v; }
};

class Axes : public osg::MatrixTransform {

  /*
   * copy world axes in target camera rotation
   */
  class AxesCallback : public osg::NodeCallback {
  protected:
    osg::Camera* mTargetCamera;

  public:
    osg::Camera* getTargetCamera() const { return mTargetCamera; }
    void setTargetCamera(osg::Camera* v) { mTargetCamera = v; }

  protected:
    virtual void operator()(osg::Node* node, osg::NodeVisitor* nv);
  };

private:
  osg::ref_ptr<osg::Geometry> mAxes;
  osg::ref_ptr<osg::AutoTransform> mLabelX;
  osg::ref_ptr<osg::AutoTransform> mLabelY;
  osg::ref_ptr<osg::AutoTransform> mLabelZ;
  osg::ref_ptr<AxesCallback> mCallback;

public:
  Axes();
  void setLabel(bool b);
  bool getLabel() { return mLabelX && mLabelX->getNumParents() != 0; }
  void setLineWidth(GLfloat w);
  GLfloat getLineWidth();

  void bindInverseView(osg::Camera* camera);
  void unbindInverseView();

private:
  osg::ref_ptr<osg::AutoTransform> createLabel(
    const osg::Vec3& v, const std::string& label);
};

/*
 * if camera ray hit something, place cursor at hit point
 * else place cursor at intersection between camra ray and plane that parallel
 * to near
 * plane and contain old cursor
 */
class CursorEventHandler : public osgGA::GUIEventHandler {
protected:
  osg::Camera* mCamera;
  osg::MatrixTransform* mCursor;

public:
  osg::Camera* getCamera() const { return mCamera; }
  void setCamera(osg::Camera* v) { mCamera = v; }

  osg::MatrixTransform* getCursor() const { return mCursor; }
  void setCursor(osg::MatrixTransform* v) { mCursor = v; }

  virtual bool handle(
    const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) {
    switch (ea.getEventType()) {
      case osgGA::GUIEventAdapter::PUSH:
        if (ea.getButton() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON) {
          osg::ref_ptr<osgUtil::LineSegmentIntersector> lsi =
            new osgUtil::LineSegmentIntersector(
              osgUtil::Intersector::WINDOW, ea.getX(), ea.getY());
          osgUtil::IntersectionVisitor iv(lsi);
          mCamera->accept(iv);

          if (lsi->containsIntersections()) {
            const osgUtil::LineSegmentIntersector::Intersection& intersection =
              *lsi->getIntersections().begin();

            // get intersection point in view space
            osg::Vec3 pos = intersection.localIntersectionPoint *
                            osg::computeLocalToWorld(intersection.nodePath);
            mCursor->setMatrix(osg::Matrix::translate(pos));
          } else {
            osg::Matrix matWorldToWnd;
            if (mCamera->getViewport())
              matWorldToWnd.preMult(
                mCamera->getViewport()->computeWindowMatrix());
            matWorldToWnd.preMult(mCamera->getProjectionMatrix());
            matWorldToWnd.preMult(mCamera->getViewMatrix());

            // get original cursor position in window space
            osg::Vec3 p0 = mCursor->getMatrix().getTrans() * matWorldToWnd;

            osg::Matrix matWndToWorld = osg::Matrix::inverse(matWorldToWnd);

            osg::Vec3 p1 =
              osg::Vec3(ea.getX(), ea.getY(), p0[2]) * matWndToWorld;

            mCursor->setMatrix(osg::Matrix::translate(p1));
          }
        }

        break;
      default:
        break;
    }
    return false;  // return true will stop event
  }
};

class Cursor : public osg::MatrixTransform {
protected:
  osg::ref_ptr<osg::Geometry> mCursor;

public:
  Cursor();
  Cursor(
    const Cursor& copy, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY)
      : osg::MatrixTransform(copy, copyop) {}
  ~Cursor() {}
  META_Object(zxd, Cursor);
};

class LightIcon : public osg::MatrixTransform {
protected:
  osg::ref_ptr<osg::Geometry> mLight;
  osg::ref_ptr<osg::Geometry> mVerticalLine;
  osg::ref_ptr<osg::MatrixTransform> mTfLine;

public:
  LightIcon();
  LightIcon(const LightIcon& copy,
    const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY)
      : osg::MatrixTransform(copy, copyop) {}
  ~LightIcon() {}
  META_Object(zxd, LightIcon);

  osg::ref_ptr<osg::MatrixTransform> getTfLine() const { return mTfLine; }
  void setTfLine(osg::ref_ptr<osg::MatrixTransform> v) { mTfLine = v; }
};

// scale vertical line until xy plane
class LightIconUpdateCallback : public osg::NodeCallback {
protected:
  virtual void operator()(osg::Node* node, osg::NodeVisitor* nv) {
    LightIcon* lightIcon = static_cast<LightIcon*>(node);
    if (!lightIcon) return;

    osg::MatrixTransform* mt = lightIcon->getTfLine();

    osg::Matrix model =
      osg::computeLocalToWorld(lightIcon->getParentalNodePaths()[0]);
    GLfloat z = model.getTrans().z();

    mt->setMatrix(osg::Matrix::scale(osg::Vec3(1.0f, 1.0f, z)));

    traverse(node, nv);
  }
};

class CameraIcon : public osg::MatrixTransform {
protected:
  osg::ref_ptr<osg::Camera> mCamera;

public:
  CameraIcon() {}
  CameraIcon(osg::Camera* camera);
  CameraIcon(const CameraIcon& copy,
    const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY)
      : osg::MatrixTransform(copy, copyop) {}
  ~CameraIcon() {}
  META_Object(zxd, CameraIcon);
};

class Dot : public osg::MatrixTransform {
protected:
  GLfloat mSize;
  osg::Vec4 mColor;
  osg::ref_ptr<osg::Geometry> mDot;

public:
  Dot(GLfloat size = 5.0f,
    const osg::Vec4 color = osg::Vec4(1.0f, 0.5f, 0.0f, 1.0f));
  Dot(const Dot& copy, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY)
      : osg::MatrixTransform(copy, copyop) {}
  ~Dot() {}
  META_Object(zxd, Dot);

  GLfloat getSize() const { return mSize; }
  void setSize(GLfloat v);

  osg::Vec4 getColor() const { return mColor; }
  void setColor(const osg::Vec4& v);
};

/*
 * used to represent constrain line
 */
class InfiniteLine : public osg::MatrixTransform {
protected:
  osg::ref_ptr<osg::Geometry> mGeometry;

public:
  InfiniteLine(
    const osg::Vec3& v = osg::X_AXIS, const osg::Vec3& color = osg::X_AXIS);
  InfiniteLine(const InfiniteLine& copy,
    const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY)
      : osg::MatrixTransform(copy, copyop) {}
  ~InfiniteLine() {}
  META_Object(zxd, InfiniteLine);
};

class LineSegmentNode : public osg::MatrixTransform {
protected:
  osg::ref_ptr<osg::Geometry> mGeometry;

public:
  LineSegmentNode();
  LineSegmentNode(const LineSegmentNode& copy,
    const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY)
      : osg::MatrixTransform(copy, copyop) {}
  ~LineSegmentNode() {}
  META_Object(zxd, LineSegmentNode);

  // reset start and end point
  void setPosition(const osg::Vec3& v0, const osg::Vec3& v1);
  void setStartPosition(const osg::Vec3& v);
  void setEndPosition(const osg::Vec3& v);
};

/*
 * point to +- y direction by default.
 * This should be used with an orthogonal camera.
 * It's orginally planed with point sprite, but it's a bit of trouble to create
 * two point sprite for this two arrows.
 */
class DirectionArrow : public osg::MatrixTransform {
protected:
  GLfloat mSize;
  GLfloat mOffset;  // offset from center, along y
  osg::ref_ptr<osg::Geometry> mGeometry;
  osg::ref_ptr<osg::MatrixTransform> mTransform0;
  osg::ref_ptr<osg::MatrixTransform> mTransform1;

public:
  DirectionArrow(GLfloat size = 16.0f);
  DirectionArrow(const DirectionArrow& copy,
    const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY)
      : osg::MatrixTransform(copy, copyop) {}
  ~DirectionArrow() {}
  META_Object(zxd, DirectionArrow);

  // d need not be normalized
  void setDirection(GLuint index, osg::Vec2 d);

  GLfloat getOffset() const { return mOffset; }
  void setOffset(GLfloat v) { mOffset = v; }

  void setColor(const osg::Vec4& color);
};
}

#endif /* AUXILIARY_H */
