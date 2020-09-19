#include <ToyMath.h>

#include <cassert>
#include <sstream>

#include <osg/DisplaySettings>

namespace toy
{

osg::Matrix createDefaultProjectionMatrix(float near, float far)
{
    double height = osg::DisplaySettings::instance()->getScreenHeight();
    double width = osg::DisplaySettings::instance()->getScreenWidth();
    double distance = osg::DisplaySettings::instance()->getScreenDistance();
    double vfov = osg::RadiansToDegrees(atan2(height / 2.0f, distance) * 2.0);

    return osg::Matrixf::perspective(vfov, width / height, near, far);
}

namespace
{

inline void setForwardUpMajor(osg::Matrix& m, const osg::Vec3& v, Axis x)
{
    auto index = static_cast<int>(x) / 2;
    if (static_cast<int>(x) % 2 == 0)
    {
        setMatrixMajor3(m, index, v);
    }
    else
    {
        setMatrixMajor3(m, index, v * -1);
    }
}

}  // namespace

osg::Matrix forwardUp(const osg::Vec3& forward, const osg::Vec3& up, Axis forwardAxis,
    Axis upAxis, Axis sideAxis)
{
    auto f = forward;
    f.normalize();

    auto s = f ^ up;
    s.normalize();

    auto u = s ^ f;
    // u.normalize();

    osg::Matrix m;

    setForwardUpMajor(m, s, sideAxis);
    setForwardUpMajor(m, u, upAxis);
    setForwardUpMajor(m, f, forwardAxis);

    return m;
}

std::vector<osg::Vec2> poissonDiskSample(
    const osg::Vec2& minExt, const osg::Vec2& maxExt, float radius, int k)
{
    std::vector<osg::Vec2> samples;
    std::vector<int> actives;
    std::vector<int> grid;

    if (minExt.x() > maxExt.x() || minExt.y() > maxExt.y())
    {
        OSG_WARN << "Illegal possion disk extent" << std::endl;
        return samples;
    }

    // step0 : Initialize an n-dimensional background grid for storing samples and
    // accelerating spatial searches. We pick the cell size to  be bounded by
    // r/sqrt(N), so that each grid cell will contain at most one sample
    auto ext = maxExt - minExt;
    auto cellSize = radius / sqrt(2.0);
    auto rows = std::ceil(ext.y() / cellSize);
    auto cols = std::ceil(ext.x() / cellSize);
    grid.assign(rows * cols, -1);
    actives.reserve(grid.size());
    samples.reserve(grid.size());

    // step1 : Select the initial sample, firstSample , randomly chosen uniformly from the
    // domain. Insert it into the background grid, and initialize the “active
    // list”
    auto firstSample = linearRand(minExt, maxExt);
    samples.push_back(firstSample);
    int row = (firstSample.y() - minExt.y()) / cellSize;
    int col = (firstSample.x() - minExt.x()) / cellSize;
    grid[row * cols + col] = 0;
    actives.push_back(0);

    // step 2 : While the active list is not empty, choose a random active_index from it
    // (say i). Generate up to k points chosen uniformly from the spherical
    // annulus between radius r and 2r around x_i . For each point in turn, check
    // if it is within distance r of existing samples (using the background grid
    // to only test nearby samples). If a point is adequately far from existing
    // samples, emit it as the next sample and add it to the active list. If after
    // k attempts no such point is found, instead remove i from the active list.
    auto nbRadius = std::ceil(sqrt(2));
    auto twoRadius = radius * 2;
    auto radius2 = radius * radius;
    while (!actives.empty())
    {
        auto activeIndex = linearRand(0.0, 1.0) * actives.size();
        const osg::Vec2& sample = samples[actives[activeIndex]];
        auto emitted = false;
        for (auto kill = k; kill > 0; --kill)
        {
            osg::Vec2 newSample =
                sample + circularRand(1.0f) * linearRand(radius, twoRadius);
            while (newSample.x() < minExt.x() || newSample.y() < minExt.y() ||
                   newSample.x() > maxExt.x() || newSample.y() > maxExt.y())
            {
                newSample = sample + circularRand(1.0f) * linearRand(radius, twoRadius);
            }

            int row = (newSample.y() - minExt.y()) / cellSize;
            int col = (newSample.x() - minExt.x()) / cellSize;

            // check neighbour of new sample to make sure there has no sample in r
            // radius
            bool validNewSample = true;
            for (auto i = -nbRadius; i <= nbRadius && validNewSample; ++i)
            {
                auto nbRow = row + i;
                if (nbRow < 0 || nbRow >= rows)
                    continue;

                for (auto j = -nbRadius; j <= nbRadius; ++j)
                {
                    auto nbCol = col + j;
                    if (nbCol < 0 || nbCol >= cols)
                        continue;

                    auto nbSampleIndex = grid[nbRow * cols + nbCol];
                    if (nbSampleIndex != -1 &&
                        (samples[nbSampleIndex] - newSample).length2() <= radius2)
                    {
                        validNewSample = false;
                        break;
                    }
                }
            }

            if (!validNewSample)
                continue;

            // debug
            // for (auto i = 0; i < samples.size(); ++i)
            // {
            //     auto r2 = (newSample - samples[i]).length2();
            //     if ( r2 <= radius2)
            //     {
            //         int c0 = (newSample.x() - minExt.x()) / cellSize;
            //         int r0 = (newSample.y() - minExt.y()) / cellSize;
            //         int c1 = (samples[i].x() - minExt.x()) / cellSize;
            //         int r1 = (samples[i].y() - minExt.y()) / cellSize;
            //         OSG_NOTICE << "found illegal newSample " << newSample.x() << ","
            //                    << newSample.y() << " " << r0 << ":" << c0
            //                    << ", it's too close to " << samples[i].x() << ","
            //                    << samples[i].y() << " " << r1 << ":" << c1 << " ("
            //                    << sqrt(r2) << "), it's grid value is "
            //                    << grid[r1 * cols + c1] << std::endl;
            //     }
            // }

            grid[row * cols + col] = samples.size();
            actives.push_back(samples.size());
            samples.push_back(newSample);
            emitted = true;
            assert(samples.size() <= grid.size());
            break;
        }

        if (!emitted)
            actives.erase(actives.begin() + activeIndex);
    }

    return samples;
}

osg::Vec4 htmlColorToVec4(const std::string& s)
{
    osg::Vec4 v(0, 0, 0, 1);
    if (s.empty())
    {
        OSG_WARN << "Empty htmp color" << std::endl;
        return v;
    }

    auto iter = s.begin();
    if (*iter == '#')
    {
        ++iter;
    }

    auto l = std::distance(iter, s.end());
    if (l != 6 && l != 8)
    {
        OSG_WARN << "Illegal html color : " << s << std::endl;
        return v;
    }

    int i = 0;
    int integer;
    std::stringstream ss;

    while (iter != s.end())
    {
        ss.seekg(0);
        ss.seekp(0);
        ss << *iter++ << *iter++;
        ss >> std::hex >> integer;
        v[i++] = integer / 255.0f;
    }

    return v;
}

bool solveQuadratic(const float& a, const float& b, const float& c, float& x0, float& x1)
{
    float discriminant = b * b - 4 * a * c;
    if (discriminant < 0)
        return false;

    if (discriminant == 0)
        x0 = x1 = -0.5 * b / a;

    // avoid catastrophic cancellation
    float q = (b > 0) ? -0.5 * (b + sqrt(discriminant)) : -0.5 * (b - sqrt(discriminant));
    x0 = q / a;
    x1 = c / q;  // x0*x1 = c / a
    if (x0 > x1)
        std::swap(x0, x1);

    return true;
}

}  // namespace toy
