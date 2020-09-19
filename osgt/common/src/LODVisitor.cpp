#include <LODVisitor.h>

#include <cassert>
#include <numeric>

#include <osg/Group>
#include <osg/LOD>
#include <osgUtil/Simplifier>

namespace toy
{

LODVisitor::LODVisitor(
    const std::vector<float>& lodDistances, const std::vector<float>& lodSampleRatios)
    : NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
    , _distances(lodDistances)
    , _sampleRatios(lodSampleRatios)
{
    _simplifier = new osgUtil::Simplifier;
}

void LODVisitor::apply(osg::Geometry& node)
{
    assert(_simplifier);
    assert(_distances.size() == _sampleRatios.size());


    auto lod = new osg::LOD;
    lod->setName(node.getName() + "LOD");
    lod->setNodeMask(node.getNodeMask());

    if (_distances.empty())
    {
        lod->addChild(&node, 0, FLT_MAX);
    }
    else
    {
        lod->addChild(&node, 0, _distances.front());
        auto rmin = _distances.front();
        for (auto i = 1; i < _distances.size(); ++i)
        {
            auto copy = dynamic_cast<osg::Geometry*>(node.clone(
                osg::CopyOp::DEEP_COPY_ARRAYS | osg::CopyOp::DEEP_COPY_PRIMITIVES));
            _simplifier->setSampleRatio(_sampleRatios[i]);
            copy->accept(*_simplifier);

            auto rmax = _distances[i];
            lod->addChild(copy, rmin, rmax);
            rmin = rmax;
        }
    }

    _lodMap[&node] = lod;

    if (_nodePath.size() >= 2)
    {
        auto& parent = dynamic_cast<osg::Group&>(*_nodePath[_nodePath.size() - 2]);
        parent.replaceChild(&node, lod);
    }
}

void LODVisitor::apply(osg::Group& node)
{
    if (!_root)
    {
        _root = &node;
    }
    NodeVisitor::apply(node);
}

osg::Node* LODVisitor::getRoot()
{
    if (_root)
    {
        return _root;
    }

    if (_lodMap.size() > 1)
    {
        OSG_WARN << "Something is wrong, no root Group found, but _lodMap has more than 1 "
                    "elements"
                 << std::endl;
    }

    return _lodMap.empty() ? 0 : _lodMap.begin()->second;
}

osg::Node* LODVisitor::filter(osg::Node& node)
{
    reset();
    node.accept(*this);
    return getRoot();
}

void LODVisitor::reset()
{
    _root = 0;
    _lodMap.clear();
}

void LODVisitor::setSimplifier(osgUtil::Simplifier* v)
{
    _simplifier = v;
}

}  // namespace toy
