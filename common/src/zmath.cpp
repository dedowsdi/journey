#include "zmath.h"
#include <stdexcept>

namespace zxd {

//------------------------------------------------------------------------------
RayRel Math::getLineRelation(const osg::Vec3& p0, const osg::Vec3& d0,
  const osg::Vec3& p1, const osg::Vec3& d1) {
  static float epsilon = 0.0005f;

  osg::Vec3 v01 = p1 - p0;
  osg::Vec3 c01 = d0 ^ d1;

  RayRel rr;

  // check parallel
  if (c01.length2() < epsilon) {
    rr.type = 0;
    return rr;
  }

  float rpl2 = 1 / c01.length2();

  rr.t0 = ((v01 ^ d1) * c01) * rpl2;
  rr.t1 = ((v01 ^ d0) * c01) * rpl2;

  rr.sk0 = p0 + d0 * rr.t0;
  rr.sk1 = p1 + d1 * rr.t1;

  // check skew or intersectiosg::Node
  rr.type = (rr.sk0 - rr.sk1).length2() > epsilon ? 2 : 1;

  return rr;
}

//------------------------------------------------------------------------------
RayPlaneRel Math::getLinePlaneRelation(
  const osg::Vec3& p0, const osg::Vec3& d0, const osg::Vec4& plane) {
  RayPlaneRel rpr;
  static float epsilon = 0.0005f;

  osg::Vec3 n(plane.x(), plane.y(), plane.z());
  float d = plane.w();

  float dotDN = d0 * n;
  if (std::abs(dotDN) < epsilon) {
    rpr.type = 0;
    return rpr;
  }

  rpr.type = 1;
  rpr.t = (-d - p0 * n) / dotDN;
  rpr.ip = p0 + d0 * rpr.t;
  return rpr;
}

//------------------------------------------------------------------------------
inline osg::Matrix Math::orthogonalizBiased(
  const osg::Matrix& m, uint startIndex /*= 0*/) {
  int i = startIndex;
  osg::Vec3 e0(m(i, 0), m(i, 1), m(i, 2));
  i = (startIndex + 1) % 3;
  osg::Vec3 e1(m(i, 0), m(i, 1), m(i, 2));
  i = (startIndex + 2) % 3;
  osg::Vec3 e2(m(i, 0), m(i, 1), m(i, 2));

  float l2e0 = e0.length2();

  e1 = e1 - e0 * (e1 * e0) / l2e0;
  e2 = e2 - e0 * (e2 * e0) / l2e0 - e1 * (e2 * e1) / e1.length2();

  return osg::Matrix(        //
    e0[0], e0[1], e0[2], 0,  // 0
    e1[0], e1[1], e1[2], 0,  // 1
    e2[0], e2[1], e2[2], 0,  // 2
    0, 0, 0, 1               // 3
    );
}

//------------------------------------------------------------------------------
osg::Matrix Math::orthogonalizIterate(const osg::Matrix& m,
  uint startIndex /*= 0*/, uint times /*= 4*/, float f /*= 0.25f*/) {
  uint i0 = startIndex;
  uint i1 = ++startIndex % 3;
  uint i2 = ++startIndex % 3;

  osg::Matrix om(m);

  for (uint i = 0; i < times; ++i) {
    osg::Vec3 e0(om(i0, 0), om(i0, 0), om(i0, 0));
    osg::Vec3 e1(om(i1, 1), om(i1, 1), om(i1, 1));
    osg::Vec3 e2(om(i2, 2), om(i2, 2), om(i2, 2));

    float l2e0 = e0.length2();
    float l2e1 = e1.length2();
    float l2e2 = e2.length2();

    e0 = e0 - e0 * (e0 * e1 * f) / l2e1 - e0 * (e0 * e2 * f) / l2e2;
    e1 = e1 - e1 * (e1 * e2 * f) / l2e1 - e1 * (e1 * e0 * f) / l2e0;
    e2 = e2 - e2 * (e2 * e0 * f) / l2e0 - e2 * (e2 * e1 * f) / l2e1;

    om = osg::Matrix(          //
      e0[0], e0[1], e0[2], 0,  // 0
      e1[0], e1[1], e1[2], 0,  // 1
      e2[0], e2[1], e2[2], 0,  // 2
      0, 0, 0, 1               // 3
      );
  }

  return om;
}
}
