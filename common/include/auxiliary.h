#ifndef AUXILIARY_H
#define AUXILIARY_H
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/State>
#include <osg/MatrixTransform>
#include <osgGA/GUIEventHandler>
#include <osgUtil/LineSegmentIntersector>

namespace zxd {

class MyDrawableCullCallback : public osg::Drawable::CullCallback {
  virtual bool cull(
    osg::NodeVisitor* nv, osg::Drawable* drawable, osg::State* state) const {
    return false;
  }
};

class FixDistanceDrawCalback : public osg::Drawable::DrawCallback {
protected:
  GLfloat mDistance;

public:
  FixDistanceDrawCalback(GLfloat distance) : mDistance(distance) {}

  virtual void drawImplementation(
    osg::RenderInfo& renderInfo, const osg::Drawable* drawable) const {
    // pre draw implementation

    osg::State* state = renderInfo.getState();
    const osg::Matrix& modelView = state->getModelViewMatrix();
    osg::Vec3 translate =
      osg::Vec3(modelView(3, 0), modelView(3, 1), modelView(3, 2));

    GLfloat l = translate.length();

    if (l == 0) {
      translate = -osg::Z_AXIS * mDistance;
    } else {
      translate = translate * (mDistance / l);
    }

    osg::Matrix m(modelView);
    m(3, 0) = translate[0];
    m(3, 1) = translate[1];
    m(3, 2) = translate[2];
    state->applyModelViewMatrix(m);

    drawable->drawImplementation(renderInfo);

    // recover modelView
    state->applyModelViewMatrix(modelView);
    // post draw Implementation
  }

  GLfloat getDistance() const { return mDistance; }
  void setDistance(GLfloat v) { mDistance = v; }
};

class FixSizeCallback : public osg::NodeCallback {
protected:
  GLfloat mDistance;
  osg::Camera* mCamera;

public:
  FixSizeCallback(GLfloat distance) : mDistance(distance), mCamera(0) {}
  virtual void operator()(osg::Node* node, osg::NodeVisitor* nv) {
    if (!mCamera) return;

    osg::Matrix matView = mCamera->getViewMatrix();
    osg::Matrix matWorld = osg::computeLocalToWorld(nv->getNodePath());

    // assume no scale exists in both matrix
    osg::Vec3 translate =
      osg::Vec3(matWorld(3, 0), matWorld(3, 1), matWorld(3, 2)) * matView;
    // assume no scale in both view and world
    GLfloat l = translate.length();

    if (l == 0) return;

    GLfloat scale = l / mDistance;

    osg::MatrixTransform* mt = node->asTransform()->asMatrixTransform();
    mt->setMatrix(osg::Matrix::scale(osg::Vec3(scale, scale, scale)));

    traverse(node, nv);
  }

  GLfloat getDistance() const { return mDistance; }
  void setDistance(GLfloat v) { mDistance = v; }

  osg::Camera* getCamera() const { return mCamera; }
  void setCamera(osg::Camera* v) { mCamera = v; }
};

class Axes : public osg::MatrixTransform {
protected:
  GLfloat mAxisSize;

  osg::ref_ptr<osg::Geometry> mAxes;
  osg::ref_ptr<FixSizeCallback> mFixSizeCallback;

public:
  Axes(osg::Camera* camera = 0);
  Axes(const Axes& copy, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY)
      : osg::MatrixTransform(copy, copyop) {}
  ~Axes() {}
  META_Object(zxd, Axes);

  GLfloat getAxisSize() const { return mAxisSize; }
  void setAxisSize(GLfloat v) { mAxisSize = v; }

  void setCamera(osg::Camera* camera) { mFixSizeCallback->setCamera(camera); }
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
            const osg::Matrix& matView = mCamera->getViewMatrix();
            // get original cursor position in view space
            osg::Vec3 origPos = mCursor->getMatrix().getTrans() * matView;

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

            mCursor->setMatrix(osg::Matrix::translate(p * matInvView));
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
  osg::ref_ptr<osg::Geometry> mInfiniteLine;

public:
  InfiniteLine(
    const osg::Vec3& v = osg::X_AXIS, const osg::Vec3& color = osg::X_AXIS);
  InfiniteLine(const InfiniteLine& copy,
    const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY)
      : osg::MatrixTransform(copy, copyop) {}
  ~InfiniteLine() {}
  META_Object(zxd, InfiniteLine);
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
  GLfloat mOffset; //offset from center, along y
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

  void setDirection(GLuint index, osg::Vec2 d);

  GLfloat getOffset() const { return mOffset; }
  void setOffset( GLfloat v){mOffset = v;}

};
}

#endif /* AUXILIARY_H */
