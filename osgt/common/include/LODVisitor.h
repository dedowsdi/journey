#ifndef WHACKAMOLE_LODVISITOR_H
#define WHACKAMOLE_LODVISITOR_H

#include <osg/NodeVisitor>

namespace osgUtil
{
class Simplifier;
}  // namespace osgUtil

namespace toy
{

// Replace all drawable with LOD. You should use getRoot() if your root node might be a
// Geometry.
class LODVisitor : public osg::NodeVisitor
{
public:
    LODVisitor(
        const std::vector<float>& lodDistances, const std::vector<float>& lodSampleRatios);

    void apply(osg::Geometry& node) override;

    void apply(osg::Group& node) override;

    const std::vector<float>& getDistances() const { return _distances; }
    void setDistances(const std::vector<float>& v) { _distances = v; }

    const std::vector<float>& getSampleRatios() const { return _sampleRatios; }
    void setSampleRatios(const std::vector<float>& v) { _sampleRatios = v; }

    osg::Node* getRoot();

    osg::Node* filter(osg::Node& node);

    void reset() override;

    osgUtil::Simplifier* getSimplifier() { return _simplifier; }
    void setSimplifier(osgUtil::Simplifier* v); 

private:
    std::vector<float> _distances;
    std::vector<float> _sampleRatios;
    osg::ref_ptr<osgUtil::Simplifier> _simplifier;
    osg::Node* _root = 0;
    std::map<osg::Geometry*, osg::Node*> _lodMap;
};

}  // namespace toy

#endif  // WHACKAMOLE_LODVISITOR_H
