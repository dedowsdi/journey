#include "glm.h"
#include <random>

namespace glm
{

//--------------------------------------------------------------------
float linearRand(float a, float b)
{
  static std::mt19937 engine;
  std::uniform_real_distribution<float> dist(a, b);
  return dist(engine);
}

//--------------------------------------------------------------------
osg::Vec2 linearRand(const osg::Vec2& a, const osg::Vec2& b)
{
  return osg::Vec2(
      linearRand(a.x(), b.x()),
      linearRand(a.y(), b.y())
      );
}

//--------------------------------------------------------------------
osg::Vec3 linearRand(const osg::Vec3& a, const osg::Vec3& b)
{
  return osg::Vec3(
      linearRand(a.x(), b.x()),
      linearRand(a.y(), b.y()),
      linearRand(a.z(), b.z())
      );
}

//--------------------------------------------------------------------
osg::Vec4 linearRand(const osg::Vec4& a, const osg::Vec4& b)
{
  return osg::Vec4(
      linearRand(a.x(), b.x()),
      linearRand(a.y(), b.y()),
      linearRand(a.z(), b.z()),
      linearRand(a.w(), b.w())
      );
}

//--------------------------------------------------------------------
osg::Vec2 circularRand(float radius)
{
  osg::Vec2 v = linearRand(osg::Vec2(-1, -1), osg::Vec2(1, 1));
  v.normalize();
  v *= radius;
  return v;
}

//--------------------------------------------------------------------
osg::Vec2 diskRand(float radius)
{
  return circularRand(radius) * linearRand(0, 1);
}

//--------------------------------------------------------------------
osg::Vec3 sphericalRand(float radius)
{
  osg::Vec3 v = linearRand(osg::Vec3(-1, -1, -1), osg::Vec3(1, 1, 1));
  v.normalize();
  v *= radius;
  return v;
}

//--------------------------------------------------------------------
osg::Vec3 ballRand(float radius)
{
  return sphericalRand(radius) * linearRand(0, 1);
}

//--------------------------------------------------------------------
float gaussRand(float mean, float deviation)
{
  static std::mt19937 engine;
  std::normal_distribution<float> dist(mean, deviation);
  return dist(engine);
}

//--------------------------------------------------------------------
osg::Vec2 gaussRand(const osg::Vec2& mean, const osg::Vec2& deviation)
{
  return osg::Vec2(gaussRand(mean[0], deviation[0]),
                   gaussRand(mean[1], deviation[1]));
}

//--------------------------------------------------------------------
osg::Vec3 gaussRand(const osg::Vec3& mean, const osg::Vec3& deviation)
{
  return osg::Vec3(gaussRand(mean[0], deviation[0]),
                   gaussRand(mean[1], deviation[1]),
                   gaussRand(mean[2], deviation[2]));
}

//--------------------------------------------------------------------
osg::Vec4 gaussRand(const osg::Vec4& mean, const osg::Vec4& deviation)
{
  return osg::Vec4(gaussRand(mean[0], deviation[0]),
                   gaussRand(mean[1], deviation[1]),
                   gaussRand(mean[2], deviation[2]),
                   gaussRand(mean[3], deviation[3]));
}

//--------------------------------------------------------------------
osg::Vec3 vec3(float x)
{
  return osg::Vec3(x, x, x);
}

//--------------------------------------------------------------------
osg::Vec4 vec4(float x)
{
  return osg::Vec4(x, x, x, x);
}

//--------------------------------------------------------------------
osg::Matrix mat4(float x)
{
  return osg::Matrix(x, 0, 0, 1,
                     0, x, 0, 1,
                     0, 0, x, 1,
                     0, 0, 0, x);
}

//--------------------------------------------------------------------
osg::Matrix mat4(const osg::Vec4& v0, const osg::Vec4& v1, const osg::Vec4& v2, const osg::Vec4& v3)
{
  return osg::Matrix(v0[0], v0[1], v0[2], v0[3],
                     v1[0], v1[1], v1[2], v1[3],
                     v2[0], v2[1], v2[2], v2[3],
                     v3[0], v3[1], v3[2], v3[3]);
}

//--------------------------------------------------------------------
osg::Matrix transpose(const osg::Matrix& m)
{
  return osg::Matrix(m(0,0), m(1,0), m(2,0), m(3,0), 
                     m(0,1), m(1,1), m(2,1), m(3,1), 
                     m(0,2), m(1,2), m(2,2), m(3,2), 
                     m(0,3), m(1,3), m(2,3), m(3,3));

}

//--------------------------------------------------------------------
float smoothstep(float edge0, float edge1, float x)
{
  float t = osg::clampTo((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
  return t*t * (3 - 2 * t);
}

//--------------------------------------------------------------------
float linearstep(float edge0, float edge1, float x)
{
  return osg::clampTo((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
}

}
