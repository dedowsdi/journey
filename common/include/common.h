#ifndef COMMON_H
#define COMMON_H

#include <osg/AnimationPath>
#include <osg/Texture>
#include <osg/Camera>
#include <osgGA/GUIEventHandler>
#include <osgText/Text>
#include <osgUtil/LineSegmentIntersector>
#include <osg/Vec3>

namespace zxd {

void getScreenResolution(
  GLuint& width, GLuint& height, GLuint screenIdentifier = 0);

extern osg::AnimationPathCallback* createAnimationPathCallback(
  float radius, float time);

extern osg::Camera* createRTTCamera(osg::Camera::BufferComponent buffer,
  osg::Texture* tex, bool isAbsolute = false);

osg::Camera* createHUDCamera(
  double left, double right, double bottom, double top);
// create 1:1 hud camera
osg::Camera* createHUDCamera(GLuint screenIdentifier = 0);

extern osg::Geode* createScreenQuad(
  float width, float height, float scale = 1.0f);

extern osgText::Text* createText(
  const osg::Vec3& pos, const std::string& content, float size);

// becareful, pos is left bottom of text, not left top
extern osgText::Text* createText(osg::ref_ptr<osgText::Font> font,
  const osg::Vec3& pos, const std::string& content, float size);

extern float randomValue(float min, float max);

extern osg::Vec3 randomVector(float min, float max);
extern osg::Vec4 randomVector4(float min, float max);

extern osg::Matrix randomMatrix(float min, float max);

// becareful this returns intrinsic rotation order.
// M = M(z)M(y)M(x)
// M = matrix.makerotate(z, y, x)
// 0 <= x <= 2pi
// 0 <= y <= pi/2 || 3pi/2 <= y <= 2pi
// 0 <= z <= 2pi
osg::Vec3 getEulerXYZ(const osg::Matrixf& m);

void removeNodeParents(osg::Node* node, GLuint count = -1);

inline GLfloat angle(const osg::Vec3& v0, const osg::Vec3& v1) {
  return acosf((v0 * v1) / (v0.length() * v1.length()));
}

inline osg::Matrix transpose(const osg::Matrix& m) {
  return osg::Matrix(                    //
    m(0, 0), m(1, 0), m(2, 0), m(3, 0),  // 0
    m(0, 1), m(1, 1), m(2, 1), m(3, 1),  // 1
    m(0, 2), m(1, 2), m(2, 2), m(3, 2),  // 2
    m(0, 3), m(1, 3), m(2, 3), m(3, 3)   // 3
    );
}

osg::Matrix arcball(
  const osg::Vec2& np0, const osg::Vec2& np1, GLfloat radius = 0.8);
osg::Vec2 screenToNdc(GLfloat x, GLfloat y, GLfloat cx, GLfloat cy);
osg::Vec2 screenToNdc(GLfloat nx, GLfloat ny);
osg::Vec3 ndcToSphere(const osg::Vec2& np0, GLfloat radius = 0.9f);

osg::Geometry* createPolygonLine(const osg::Vec3& v, GLfloat size);

class PickHandler : public osgGA::GUIEventHandler {
public:
  virtual bool handle(
    const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);
  virtual void doUserOperations(
    osgUtil::LineSegmentIntersector::Intersection& result) = 0;
};

const osg::Vec3f ORIGIN(0.0f, 0.0f, 0.0f);
}

#endif /* COMMON_H */
