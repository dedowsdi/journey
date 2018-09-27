#ifndef __GLM_H
#define __GLM_H

#include <cmath>
#include <osg/Vec2>
#include <osg/Vec3>
#include <osg/Vec4>
#include <osg/Matrix>

namespace glm
{
  float linearRand(float a, float b);
  osg::Vec2 linearRand(const osg::Vec2& a, const osg::Vec2& b);
  osg::Vec3 linearRand(const osg::Vec3& a, const osg::Vec3& b);
  osg::Vec4 linearRand(const osg::Vec4& a, const osg::Vec4& b);
  osg::Vec2 circleRand(float radius);
  osg::Vec2 diskRand(float radius);
  osg::Vec3 sphericalRand(float radius);
  osg::Vec3 ballRand(float radius);
  float gaussRand(float mean, float deviation);
  osg::Vec2 gaussRand(const osg::Vec2& mean, const osg::Vec2& deviation);
  osg::Vec3 gaussRand(const osg::Vec3& mean, const osg::Vec3& deviation);
  osg::Vec4 gaussRand(const osg::Vec4& mean, const osg::Vec4& deviation);

  osg::Vec3 vec3(float x);
  osg::Vec4 vec4(float x);

  osg::Matrix mat4(float x); // diagonal
  osg::Matrix mat4(const osg::Vec4& v0,const osg::Vec4& v1,const osg::Vec4& v2,const osg::Vec4& v3);

  float smoothstep(float edge0, float edge1, float x);

  template<typename T, typename  U>
  T mix(const T& x, const T& y, U a)
  {
    return x*(1-a) + y*a;
  }

  const float fpi = 3.14159265358979323846264338327950288419716939937510;
  const float f2pi = fpi * 2;
  const float fpi2 = fpi / 2;
  const float fpi4 = fpi / 4;
  const double dpi = 3.14159265358979323846264338327950288419716939937510;
  const float d2pi = dpi * 2;
  const float dpi2 = dpi / 2;
  const float dpi4 = dpi / 4;
}


#endif /* __GLM_H */
