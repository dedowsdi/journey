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
osg::Vec3 sphericalRand(float radius)
{
  osg::Vec3 v = linearRand(osg::Vec3(-1, -1, -1), osg::Vec3(1, 1, 1));
  v.normalize();
  v *= radius;
  return v;
}

//--------------------------------------------------------------------
float smoothstep(float edge0, float edge1, float x)
{
  float t = osg::clampTo((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
  return t*t * (3 - 2 * t);
}

}
