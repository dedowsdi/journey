#include <OsgFactory.h>
#include <Screw.h>

namespace toy
{

void Screw::revolve(
    const std::vector<osg::Vec3>& points, float angle, const osg::Vec3& axis, int steps)
{
    _points = points;
    _axis = axis;
    _angle = angle;
    _steps = steps;

    auto vertices = new osg::Vec3Array(points.begin(), points.end());
    vertices->setBinding(osg::Array::BIND_PER_VERTEX);
    vertices->reserve(points.size() * (steps + 1));

    auto close = std::abs(std::abs(angle) - osg::PI_2) <= 0.0001;
    for (auto i = 1; i <= steps; ++i)
    {
        if (i == steps && close)
        {
            vertices->insert(vertices->end(), points.begin(), points.end());
        }
        else
        {
            osg::Quat quat(i * angle, axis);
            for (auto j = 0; j < points.size(); ++j)
            {
                vertices->push_back(quat * points[j]);
            }
        }
    }

    auto texcoords = new osg::Vec2Array(osg::Array::BIND_PER_VERTEX);
    osgf::addGridTexcoordsSliceBySlice(*texcoords, steps + 1, points.size());
    osgf::addGridElements<osg::DrawElementsUInt>(*this, steps + 1, points.size());
}

}  // namespace toy
