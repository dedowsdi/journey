#ifndef WHACKAMOLE_SCREW_H
#define WHACKAMOLE_SCREW_H

#include <osg/Geometry>

namespace toy
{

class Screw : public osg::Geometry
{
public:
    Screw() {}

    Screw(const Screw& src, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY) {}

    META_Node(toy, Screw);

    void revolve(const std::vector<osg::Vec3>& points, float angle, const osg::Vec3& axis,
        int steps);

private:
    int _steps = 16;
    float _angle = 0;
    osg::Vec3 _axis;
    std::vector<osg::Vec3> _points;
};

}  // namespace toy

#endif  // WHACKAMOLE_SCREW_H
