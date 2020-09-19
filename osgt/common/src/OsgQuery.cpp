#include <OsgQuery.h>

#include <algorithm>
#include <climits>

#include <osg/Camera>
#include <osgAnimation/Timeline>
#include <osgUtil/LineSegmentIntersector>
#include <osgViewer/Viewer>

namespace osgq
{

Segment getCameraRay(
    osg::Camera& camera, double winX, double winY, float startDepth, float endDepth)
{
    auto vpwMat = camera.getViewMatrix() * camera.getProjectionMatrix() *
                  camera.getViewport()->computeWindowMatrix();
    auto vpwMatI = osg::Matrix::inverse(vpwMat);

    auto start = osg::Vec3(winX, winY, startDepth) * vpwMatI;
    auto end = osg::Vec3(winX, winY, endDepth) * vpwMatI;

    return std::make_pair(start, end);
}

bool contains(osgAnimation::Timeline& timeline, osgAnimation::Action& action)
{
    auto actions = timeline.getActionLayer(1);
    auto iter = std::find_if(actions.begin(), actions.end(),
        [&action](osgAnimation::FrameAction& fa) { return fa.second == &action; });
    return iter != actions.end();
}

class SearchNodeVisitor : public osg::NodeVisitor
{
public:
    SearchNodeVisitor(const std::string& name, int maxDepth)
        : _maxDepth(maxDepth), _name(name)
    {
        setTraversalMode(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN);
    }

    void apply(osg::Node& node) override
    {
        if (_depth >= 0 && node.getName() == _name)
        {
            _nodes.push_back(&node);
        }

        if (_depth < _maxDepth)
        {
            ++_depth;
            traverse(node);
            --_depth;
        }
    }

    std::vector<osg::Node*>  getNodes() const { return _nodes; }

private:
    // Use the same convention as find, depth 0 means search direct children
    int _maxDepth = 0;
    int _depth = -1;
    std::vector<osg::Node*> _nodes ;
    std::string _name;
};

std::vector<osg::Node*> searchNode(osg::Node& node, const std::string& name, int maxDepth)
{
    SearchNodeVisitor visitor(name, maxDepth < 0 ? INT_MAX : maxDepth);
    visitor.setNodeMaskOverride(-1);
    node.accept(visitor);
    return visitor.getNodes();
}

class SearchNodeByMaterialVisitor : public osg::NodeVisitor
{
public:
    SearchNodeByMaterialVisitor(const std::string& name)
        : _name(name)
    {
        setTraversalMode(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN);
    }

    void apply(osg::Node& node) override
    {
        auto ss = node.getStateSet();
        if (ss)
        {
            auto mtl = ss->getAttribute(osg::StateAttribute::MATERIAL);
            if (mtl && mtl->getName() == _name)
            {
                _nodes.push_back(&node);
            }
        }
        traverse(node);
    }

    std::vector<osg::Node*>  getNodes() const { return _nodes; }

private:
    // Use the same convention as find, depth 0 means search direct children
    std::vector<osg::Node*> _nodes ;
    std::string _name;
};

std::vector<osg::Node*> searchNodeByMaterial(osg::Node& node, const std::string& name)
{
    SearchNodeByMaterialVisitor visitor(name);
    visitor.setNodeMaskOverride(-1);
    node.accept(visitor);
    return visitor.getNodes();
}

namespace
{

template<typename T>
class SearchNodeTypeVisitor : public osg::NodeVisitor
{
public:
    using AsFunc = T* (osg::Node::*)();

    SearchNodeTypeVisitor(AsFunc asFunc) : _asFunc(asFunc)
    {
        setTraversalMode(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN);
    }

    void apply(osg::Node& node) override
    {
        T* t = (node.*_asFunc)();
        if (t)
        {
            _nodePaths.push_back(getNodePath());
        }
        traverse(node);
    }

    const osg::NodePathList& getNodePathList() const { return _nodePaths; }
    osg::NodePathList&& takeNodePathList() { return move(_nodePaths); }

private:
    std::vector<T*> _nodes;
    osg::NodePathList _nodePaths;
    AsFunc _asFunc;
};

}  // namespace

template<typename T>
osg::NodePathList searchType(osg::Node& node, T* (osg::Node::*asFunc)(), int traversalMask)
{
    SearchNodeTypeVisitor<T> visitor(asFunc);
    visitor.setTraversalMask(traversalMask);
    node.accept(visitor);
    return visitor.takeNodePathList();
}

#define INSTANTIATE_searchType(T)                                                         \
    template osg::NodePathList searchType<T>(osg::Node&, T * (osg::Node::*)(), int);

INSTANTIATE_searchType(osg::Drawable);
INSTANTIATE_searchType(osg::Geometry);
INSTANTIATE_searchType(osg::Group);
INSTANTIATE_searchType(osg::Transform);
INSTANTIATE_searchType(osg::Switch);
INSTANTIATE_searchType(osg::Geode);
INSTANTIATE_searchType(osgTerrain::Terrain);

void* getGraphicsWindow(const osgViewer::Viewer& viewer)
{
    osgViewer::Viewer::Windows windows;
    const_cast<osgViewer::Viewer&>(viewer).getWindows(windows);
    if (windows.empty())
    {
        throw std::runtime_error("Failed to get window");
    }
    return windows[0];
}

const void* getGraphicsContextTraits(const osgViewer::Viewer& viewer)
{
    return static_cast<const osgViewer::GraphicsWindow*>(getGraphicsWindow(viewer))
        ->getTraits();
}

osg::Vec4i getWindowRect(const osgViewer::Viewer& viewer)
{
    auto traits =
        static_cast<const osg::GraphicsContext::Traits*>(getGraphicsContextTraits(viewer));
    return osg::Vec4i(traits->x, traits->y, traits->width, traits->height);
}

osg::Vec2i getScreenSize(int identifier)
{
    auto wsi = osg::GraphicsContext::getWindowingSystemInterface();
    if (!wsi)
    {
        throw std::runtime_error("failed to get window system interface");
    }

    osg::GraphicsContext::ScreenIdentifier si(identifier);
    si.readDISPLAY();
    si.setUndefinedScreenDetailsToDefaultScreen();

    unsigned width, height;
    wsi->getScreenResolution(si, width, height);

    return osg::Vec2i(width, height);
}

osg::ref_ptr<osgUtil::LineSegmentIntersector> pickNearest(
    osg::Camera& camera, float x, float y, int traversalMask)
{
    auto picker = new osgUtil::LineSegmentIntersector(osgUtil::Intersector::WINDOW, x, y);
    picker->setIntersectionLimit(osgUtil::Intersector::LIMIT_NEAREST);

    osgUtil::IntersectionVisitor iv;
    iv.setTraversalMask(traversalMask);
    iv.setIntersector(picker);

    camera.accept(iv);

    return picker;
}

}  // namespace osgq
