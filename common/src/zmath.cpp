#include "zmath.h"
#include <stdexcept>

namespace zxd {

//------------------------------------------------------------------------------
LineRelPair Math::getLineRelation(const osg::Vec3& p0, const osg::Vec3& d0,
  const osg::Vec3& p1, const osg::Vec3& d1) {
  static float epsilon = 0.0005f;

  osg::Vec3 v01 = p1 - p0;
  osg::Vec3 c01 = d0 ^ d1;

  // check parallel
  if (c01.length2() < epsilon)
    return std::make_pair(0, std::make_pair(osg::Vec3(), osg::Vec3()));

  float rpl2 = 1 / c01.length2();

  float t0 = ((v01 ^ d1) * c01) * rpl2;
  float t1 = ((v01 ^ d0) * c01) * rpl2;

  osg::Vec3 ip0 = p0 + d0 * t0;
  osg::Vec3 ip1 = p1 + d1 * t1;

  // check skew or intersectiosg::Node
  int result = (ip0 - ip1).length2() > epsilon ? 2 : 1;

  return std::make_pair(result, std::make_pair(ip0, ip1));
}

//------------------------------------------------------------------------------
LinePlaneRelPair Math::getLinePlaneRelation(
  const osg::Vec3& p0, const osg::Vec3& d0, const osg::Vec4& plane) {
  static float epsilon = 0.0005f;

  osg::Vec3 n(plane.x(), plane.y(), plane.z());
  float d = plane.w();

  float dotDN = d0 * n;
  if (std::abs(dotDN) < epsilon) return std::make_pair(0, osg::Vec3());

  float t = (-d - p0 * n) / dotDN;
  return std::make_pair(1, osg::Vec3(p0 + d0 * t));
}
}
