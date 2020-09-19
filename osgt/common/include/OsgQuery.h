#ifndef BOUNCINGBALL_OSGTEST_H
#define BOUNCINGBALL_OSGTEST_H

// Test, get, search util for osg, don't include large head files. If you need to return
// inner class handle, use void* instead.
#include <string>
#include <vector>

#include <osg/Matrix>
#include <osg/Vec2>
#include <osg/Vec2i>
#include <osg/Vec3>
#include <osg/Vec4>
#include <osg/Vec4i>

namespace osg
{
class Camera;
class Group;
class Node;
class Drawable;
class Geometry;
class Group;
class Transform;
class Switch;
class Geode;
class ArgumentParser;

typedef std::vector<Node*> NodePath;
typedef std::vector<NodePath> NodePathList;
}  // namespace osg

namespace osgAnimation
{
class Timeline;
class Action;
};  // namespace osgAnimation

namespace osgViewer
{
class Viewer;
}

namespace osgTerrain
{
class Terrain;
}

namespace osgUtil
{
class LineSegmentIntersector;
}

namespace osgq
{

// Space {{{1

using Segment = std::pair<osg::Vec3, osg::Vec3>;
Segment getCameraRay(osg::Camera& camera, double winX, double winY, float startDepth = 0,
    float endDepth = 1);

// Animation {{{1

bool contains(osgAnimation::Timeline& timeline, osgAnimation::Action& action);

// Node {{{1

// use negative maxDepth if you want unlimited depth.
std::vector<osg::Node*> searchNode(
    osg::Node& node, const std::string& name, int maxDepth = -1);

std::vector<osg::Node*> searchNodeByMaterial(osg::Node& node, const std::string& name);

template<typename T>
osg::NodePathList searchType(
    osg::Node& node, T* (osg::Node::*asFunc)(), int traversalMask = -1);

// Window {{{1

void* getGraphicsWindow(const osgViewer::Viewer& viewer);

// Return osg::GraphicsContext::Traits*
const void* getGraphicsContextTraits(const osgViewer::Viewer& viewer);

// x y with height
osg::Vec4i getWindowRect(const osgViewer::Viewer& viewer);

osg::Vec2i getScreenSize(int identifier = 0);

osg::ref_ptr<osgUtil::LineSegmentIntersector> pickNearest(
    osg::Camera& camera, float x, float y, int traversalMask = -1);

}  // namespace osgq

#endif  // BOUNCINGBALL_OSGTEST_H
