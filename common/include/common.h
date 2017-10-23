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

typedef std::vector<osg::ref_ptr<osg::Vec3Array>> Vec3ArrayVec;

void getScreenResolution(
  GLuint& width, GLuint& height, GLuint screenIdentifier = 0);

extern osg::AnimationPathCallback* createAnimationPathCallback(
  float radius, float time);

extern osg::Camera* createRTTCamera(osg::Camera::BufferComponent buffer,
  osg::Texture* tex, bool isAbsolute = false);

osg::Camera* createHUDCamera(double left, double right, double bottom,
  double top, double near, double far);
// create 1:1 hud camera
osg::Camera* createHUDCamera(
  GLuint screenIdentifier = 0, double near = -1, double far = 1);

extern osg::Geode* createScreenQuad(
  float width, float height, float scale = 1.0f);

extern osgText::Text* createText(
  const osg::Vec3& pos, const std::string& content, float size);

// becareful, pos is left bottom of text, not left top
extern osgText::Text* createText(osg::ref_ptr<osgText::Font> font,
  const osg::Vec3& pos, const std::string& content, float size);

osg::ref_ptr<osg::Geometry> createSingleDot(GLfloat pointSize = 1.0f,
  const osg::Vec4& color = osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));

void removeNodeParents(osg::Node* node, GLuint count = -1);

double getBestFovy();

osg::Geometry* createPolygonLine(const osg::Vec3& v, GLfloat size);

class PickHandler : public osgGA::GUIEventHandler {
public:
  virtual bool handle(
    const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);
  virtual void doUserOperations(
    osgUtil::LineSegmentIntersector::Intersection& result) = 0;
  virtual void doNoHit(){};
};

const osg::Vec3f ORIGIN(0.0f, 0.0f, 0.0f);
}

#endif /* COMMON_H */
