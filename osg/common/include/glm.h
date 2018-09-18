#ifndef __GLM_H
#define __GLM_H

#include <cmath>
#include <osg/Vec2>
#include <osg/Vec3>
#include <osg/Vec4>

namespace glm
{
  float linearRand(float a, float b);
  osg::Vec2 linearRand(const osg::Vec2& a, const osg::Vec2& b);
  osg::Vec3 linearRand(const osg::Vec3& a, const osg::Vec3& b);
  osg::Vec4 linearRand(const osg::Vec4& a, const osg::Vec4& b);

  osg::Vec3 sphericalRand(float radius);

  template<typename T, typename  U>
  T mix(const T& x, const T& y, U a)
  {
    return x*(1-a) + y*a;
  }

  float smoothstep(float edge0, float edge1, float x);

  const float fpi = 3.1415926;
  const float f2pi = fpi * 2;
  const float fpi2 = fpi / 2;
  const float fpi4 = fpi / 4;
  const double dpi = 3.1415926;
  const float d2pi = dpi * 2;
  const float dpi2 = dpi / 2;
  const float dpi4 = dpi / 4;
}


#endif /* __GLM_H */
