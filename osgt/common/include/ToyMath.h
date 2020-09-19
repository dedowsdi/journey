#ifndef BOUNCINGBALL_MATH_H
#define BOUNCINGBALL_MATH_H

#include <random>

#include <osg/Matrix>
#include <osg/Vec2>
#include <osg/Vec3>
#include <osg/Vec4>

namespace toy
{

inline std::mt19937& getPsudoGenerator()
{
    static auto gen = []() {
        std::random_device rd;
        auto seed = rd();
        OSG_NOTICE << "seed " << seed << std::endl;
        return std::mt19937(rd());
    }();
    return gen;
}

// [0,1)
inline float unitRand()
{
    static std::uniform_real_distribution<> dist(0.0f, 1.0f);
    return dist(getPsudoGenerator());
}

inline float gaussRand(float mean, float stddev)
{
    std::normal_distribution<> dist(mean, stddev);
    return dist(getPsudoGenerator());
}

inline float gaussRand(const osg::Vec2& v)
{
    return gaussRand(v.x(), v.y());
}

// [a, b)
inline float linearRand(float a, float b)
{
    return a + unitRand() * (b - a);
}

namespace detail
{
// base class to test has num_components, U defaults to int, which is specialized, so you
// can use Has_num_components<T>::value to test if it has num_components, SFINAE will ignore
// the specialization if it doesn't have one.
// check https://stackoverflow.com/a/16000226/4922642 for more detail
template<typename T, typename U = int>
struct Has_num_components : std::false_type
{
};

// specialize for U = int. decltype((void)T::num_components, 0) is a fancy way to say int
// with SFINAE, (void) is used to avoid , overload between between T::num_components and 0.
template<typename T>
struct Has_num_components<T, decltype((void)T::num_components, 0)> : std::true_type
{
};
}  // namespace detail

// exclude primitive and class without num_components member.
#define VEC_T std::enable_if_t<detail::Has_num_components<T>::value, T>
#define NOT_VEC_T std::enable_if_t<!detail::Has_num_components<T>::value, T>

template<typename T>
inline VEC_T linearRand(const T& a, const T& b)
{
    T t;
    for (auto i = 0; i < T::num_components; ++i)
    {
        t[i] = linearRand(a[i], b[i]);
    }

    return t;
}

template<typename T>
inline NOT_VEC_T clamp(T v, T minimum, T maximum)
{
    return v < minimum ? minimum : v > maximum ? maximum : v;
}

template<typename T>
inline VEC_T clamp(const T& v, const T& minimum, const T& maximum)
{
    T t;
    for (auto i = 0; i < T::num_components; ++i)
    {
        t[i] = osg::clampTo(v[i], minimum[i], maximum[i]);
    }
    return t;
}

template<typename T>
inline VEC_T clamp(
    const T& v, typename T::value_type minimum, typename T::value_type maximum)
{
    T t;
    for (auto i = 0; i < T::num_components; ++i)
    {
        t[i] = clamp(v[i], minimum, maximum);
    }
    return t;
}

template<typename T>
inline VEC_T abs(const T& v)
{
    T t;
    for (auto i = 0; i < T::num_components; ++i)
    {
        t[i] = std::abs(v[i]);
    }
    return t;
}

template<typename T>
inline VEC_T mod(const T& a, const T& b)
{
    T t;
    for (auto i = 0; i < T::num_components; ++i)
    {
        t[i] = std::fmod(a[i], b[i]);
    }
    return t;
}

template<typename T>
inline VEC_T mod(const T& a, typename T::value_type p)
{
    T t;
    for (auto i = 0; i < T::num_components; ++i)
    {
        t[i] = std::fmod(a[i], p);
    }
    return t;
}

template<typename T>
inline T mix(T a, T b, T p)
{
    return a * (1 - p) + b * p;
}

template<typename T>
inline VEC_T mix(const T& a, const T& b, const T& p)
{
    T t;
    for (auto i = 0; i < T::num_components; ++i)
    {
        t[i] = mix(a[i], b[i], p[i]);
    }
    return t;
}

template<typename T>
inline VEC_T mix(const T& a, const T& b, typename T::value_type p)
{
    T t;
    for (auto i = 0; i < T::num_components; ++i)
    {
        t[i] = mix(a[i], b[i], p);
    }
    return t;
}

template<typename T>
inline VEC_T ceil(const T& a)
{
    T t;
    for (auto i = 0; i < T::num_components; ++i)
    {
        t[i] = ceil(a[i]);
    }
    return t;
}

template<typename T>
inline VEC_T floor(const T& a)
{
    T t;
    for (auto i = 0; i < T::num_components; ++i)
    {
        t[i] = floor(a[i]);
    }
    return t;
}

inline osg::Vec3 hsv2rgb(const osg::Vec3& c)
{
    auto x6 = c.x() * 6.0f;
    osg::Vec3 rgb =
        clamp(abs(mod(osg::Vec3(x6, x6, x6) + osg::Vec3(0.0f, 4.0f, 2.0f), 6.0f) -
                  osg::Vec3(3.0f, 3.0f, 3.0f)) -
                  osg::Vec3(1.0f, 1.0f, 1.0f),
            0.0f, 1.0f);

    return mix(osg::Vec3(1.0, 1.0, 1.0), rgb, c.y()) * c.z();
}

inline osg::Vec2 circularRand(float radius)
{
    auto v = linearRand(osg::Vec2(-1, -1), osg::Vec2(1, 1));
    v.normalize();
    v *= radius;
    return v;
}

inline osg::Vec2 ringRand(float inner_radius, float outer_radius)
{
    return linearRand(circularRand(inner_radius), circularRand(outer_radius));
}

inline osg::Vec2 circularRand(float radius, float startAngle, float endAngle)
{
    auto theta = linearRand(startAngle, endAngle);
    return osg::Vec2(cos(theta), sin(theta)) * radius;
}

inline osg::Vec2 diskRand(float radius)
{
    return circularRand(radius) * linearRand(0.0f, 1.0f);
}

// https://www.bogotobogo.com/Algorithms/uniform_distribution_sphere.php
inline osg::Vec3 sphericalRand(float radius)
{
    // place sphere in a right circle cylinder, there area is the same. cut sphere by 2
    // parallels, one of them pass origin, the other is h distance away, the strip area is
    // exactly 2πrh, which is also the same as cylinder cut. So we peek random point
    // on cylinder first, than related point on sphere, it's theta is acos(cylinder_pos.z)
    auto theta = acos(linearRand(-1.0f, 1.0f));
    auto phi = unitRand() * osg::PI * 2;
    auto sinTheta = sin(theta);
    return osg::Vec3(sinTheta * cos(phi), sinTheta * sin(phi), cos(theta)) * radius;
}

inline osg::Vec3 sphericalRand(
    float radius, const osg::Vec2& thetaRange, const osg::Vec2& phiRange)
{
    auto theta = acos(linearRand(-1.0f, 1.0f));
    auto p = theta / osg::PI;
    theta = thetaRange.x() * (1 - p) + thetaRange.y() * p;

    p = unitRand();
    auto phi = phiRange.x() * (1 - p ) + phiRange.y() * p;

    auto sinTheta = sin(theta);
    return osg::Vec3(sinTheta * cos(phi), sinTheta * sin(phi), cos(theta)) * radius;
}

inline osg::Vec3 ballRand(float radius)
{
    return sphericalRand(radius) * linearRand(0.0f, 1.0f);
}

// Assume ndir normalized
inline osg::Vec3 randomOrghogonal(const osg::Vec3& ndir)
{
    // find a reference vector that's not parallel to ndir
    auto ref = sphericalRand(1.0);
    while (std::abs(ndir * ref) > 0.999f)
    {
        ref = sphericalRand(1.0);
    }

    auto perp = ndir ^ ref;
    return perp;
}

osg::Matrix createDefaultProjectionMatrix(float near, float far);

enum class Axis
{
    X,
    NX,
    Y,
    NY,
    Z,
    NZ
};

// The ubiquitous forward up, defaults to lookup style, although it return camera frame
// instead of it's inverse(view matrix).
osg::Matrix forwardUp(const osg::Vec3& forward, const osg::Vec3& up,
    Axis forwardAxis = Axis::NZ, Axis upAxis = Axis::Y, Axis sideAxis = Axis::X);

inline osg::Matrix forwardUpOsg(const osg::Vec3& forward, const osg::Vec3& up)
{
    return forwardUp(forward, up, Axis::Y, Axis::Z, Axis::X);
}

template<typename MatrixType = osg::Matrixf>
struct MatrixVec
{
    using Vec3 = osg::Vec3f;
    using Vec4 = osg::Vec4f;
};

template<>
struct MatrixVec<osg::Matrixd>
{
    using Vec3 = osg::Vec3d;
    using Vec4 = osg::Vec4d;
};

template<typename T>
using MVec3T = typename MatrixVec<T>::Vec3;

template<typename T>
using MVec4T = typename MatrixVec<T>::Vec4;

inline MVec4T<osg::Matrix> getMatrixMajor(const osg::Matrix& m, int index)
{
    return MVec4T<osg::Matrix>(m(index, 0), m(index, 1), m(index, 2), m(index, 3));
}

inline MVec3T<osg::Matrix> getMatrixMajor3(const osg::Matrix& m, int index)
{
    return MVec3T<osg::Matrix>(m(index, 0), m(index, 1), m(index, 2));
}

inline MVec4T<osg::Matrix> getMatrixMinor(const osg::Matrix& m, int index)
{
    return MVec4T<osg::Matrix>(m(0, index), m(1, index), m(2, index), m(3, index));
}

inline MVec3T<osg::Matrix> getMatrixMinor3(const osg::Matrixf& m, int index)
{
    return MVec3T<osg::Matrix>(m(0, index), m(1, index), m(2, index));
}

inline void setMatrixMajor(osg::Matrix& m, int index, const MVec4T<osg::Matrix>& axis)
{
    m(index, 0) = axis[0];
    m(index, 1) = axis[1];
    m(index, 2) = axis[2];
    m(index, 3) = axis[3];
}

// Overload can be dangerous here, I prefer to use a different name.
inline void setMatrixMajor3(osg::Matrix& m, int index, const MVec3T<osg::Matrix>& axis)
{
    m(index, 0) = axis[0];
    m(index, 1) = axis[1];
    m(index, 2) = axis[2];
}

inline void setMatrixMinor(osg::Matrix& m, int index, const MVec4T<osg::Matrix>& axis)
{
    m(0, index) = axis[0];
    m(1, index) = axis[1];
    m(2, index) = axis[2];
    m(3, index) = axis[3];
}

// Overload can be dangerous here, I prefer to use a different name.
inline void setMatrixMinor3(osg::Matrix& m, int index, const MVec3T<osg::Matrix>& axis)
{
    m(0, index) = axis[0];
    m(1, index) = axis[1];
    m(2, index) = axis[2];
}

template<typename T>
inline T createMatrix(const MVec4T<T>& axis0, const MVec4T<T>& axis1,
    const MVec4T<T>& axis2, const MVec4T<T>& axis3)
{
    // clang-format off
    return T( axis0[0], axis0[1], axis0[2], axis0[3], 
              axis1[0], axis1[1], axis1[2], axis1[3], 
              axis2[0], axis2[1], axis2[2], axis2[3], 
              axis3[0], axis3[1], axis3[2], axis3[3]);
    // clang-format on
}

template<typename T>
inline T createMatrix(
    const MVec3T<T>& axis0, const MVec3T<T>& axis1, const MVec3T<T>& axis2)
{
    // clang-format off
    return T( axis0[0], axis0[1], axis0[2], 0,
              axis1[0], axis1[1], axis1[2], 0,
              axis2[0], axis2[1], axis2[2], 0,
              0,        0,        0,        1);
    // clang-format on
}

template<typename T>
inline T createMatrixMinor(const MVec4T<T>& axis0, const MVec4T<T>& axis1,
    const MVec4T<T>& axis2, const MVec4T<T>& axis3)
{
    // clang-format off
    return T( axis0[0], axis1[0], axis2[0], axis3[0],
              axis0[1], axis1[1], axis2[1], axis3[1],
              axis0[2], axis1[2], axis2[2], axis3[2],
              axis0[3], axis1[3], axis2[3], axis3[3]);
    // clang-format on
}

template<typename T>
inline T createMatrixMinor(
    const MVec3T<T>& axis0, const MVec3T<T>& axis1, const MVec3T<T>& axis2)
{
    // clang-format off
    return T( axis0[0], axis1[0], axis2[0], 0,
              axis0[1], axis1[1], axis2[1], 0,
              axis0[2], axis1[2], axis2[2], 0,
              0,        0,        0,        1);
    // clang-format on
}

std::vector<osg::Vec2> poissonDiskSample(
    const osg::Vec2& minExt, const osg::Vec2& maxExt, float radius, int k);

inline float smoothstep(float edge0, float edge1, float x)
{
    float t;
    t = clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
    return t * t * (3.0 - 2.0 * t);
}

osg::Vec4 htmlColorToVec4(const std::string& s);

// x0 <= x1
bool solveQuadratic(const float& a, const float& b, const float& c, float& x0, float& x1);

}  // namespace toy

#endif  // BOUNCINGBALL_MATH_H
