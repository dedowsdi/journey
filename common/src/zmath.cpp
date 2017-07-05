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

//------------------------------------------------------------------------------
osg::Vec3 Math::getEulerXYZ(const osg::Matrixf& mat) {
  // http://www.j3d.org/matrix_faq/
  GLfloat angle_y, D, C, trx, _try, angle_x, angle_z;
  angle_y = D = asin(mat(2, 0)); /* Calculate Y-axis angle */
  C = cos(angle_y);
  // angle_y    *=  RADIANS;
  if (fabs(C) > 0.005) /* Gimball lock? */
  {
    trx = mat(2, 2) / C; /* No, so get X-axis angle */
    _try = -mat(2, 1) / C;
    angle_x = atan2(_try, trx);
    trx = mat(0, 0) / C; /* Get Z-axis angle */
    _try = -mat(1, 0) / C;
    angle_z = atan2(_try, trx);
  } else /* Gimball lock has occurred */
  {
    angle_x = 0;     /* Set X-axis angle to zero */
    trx = mat(1, 1); /* And calculate Z-axis angle */
    _try = mat(0, 1);
    angle_z = atan2(_try, trx);
  }

  /* return only positive angles in [0,360] */
  if (angle_x < 0) angle_x += osg::PI * 2;
  if (angle_y < 0) angle_y += osg::PI * 2;
  if (angle_z < 0) angle_z += osg::PI * 2;

  return osg::Vec3(angle_x, angle_y, angle_z);
}

//------------------------------------------------------------------------------
osg::Matrix Math::arcball(
  const osg::Vec2& np0, const osg::Vec2& np1, GLfloat radius /*= 0.8*/) {
  // get camera point
  osg::Vec3 sp0 = ndcToSphere(np0, radius);
  osg::Vec3 sp1 = ndcToSphere(np1, radius);
  GLfloat rpRadius = 1 / radius;
  // get rotate axis in camera space
  osg::Vec3 axis = sp0 ^ sp1;
  GLfloat theta = acosf(sp0 * sp1 * rpRadius * rpRadius);

  return osg::Matrix::rotate(theta, axis);
}

//------------------------------------------------------------------------------
osg::Vec2 Math::screenToNdc(GLfloat x, GLfloat y, GLfloat cx, GLfloat cy) {
  return osg::Vec2((2 * x - cx) / cx, (cy - 2 * y) / cy);
}

//------------------------------------------------------------------------------
osg::Vec2 Math::screenToNdc(GLfloat nx, GLfloat ny) {
  return osg::Vec2(2 * nx - 1, 2 * ny - 1);
}

//------------------------------------------------------------------------------
osg::Vec3 Math::ndcToSphere(const osg::Vec2& np0, GLfloat radius /*= 0.9f*/) {
  GLfloat len2 = np0.length2();
  GLfloat radius2 = radius * radius;
  if (len2 >= radius2) {
    return osg::Vec3(np0 * (radius / std::sqrt(len2)), 0);
  } else {
    return osg::Vec3(np0, std::sqrt(radius2 - len2));
  }
}

//------------------------------------------------------------------------------
osg::BoundingBox Math::transformAABB(
  const osg::BoundingBox& aabb, const osg::Matrix& m) {
  // it's not necessary to transfrom all 8 vertices to get result aabb
  // every component of matrix will times min or max element of bounding box
  // corner
  double xMin = 0.0f, xMax = 0.0f, yMin = 0.0f, yMax = 0.0f, zMin = 0.0f,
         zMax = 0.0f;

  addMinMax(xMin, xMax, aabb.xMin() * m(0, 0), aabb.xMax() * m(0, 0));
  addMinMax(xMin, xMax, aabb.yMin() * m(1, 0), aabb.yMax() * m(1, 0));
  addMinMax(xMin, xMax, aabb.zMin() * m(2, 0), aabb.zMax() * m(2, 0));

  addMinMax(yMin, yMax, aabb.xMin() * m(0, 1), aabb.xMax() * m(0, 1));
  addMinMax(yMin, yMax, aabb.yMin() * m(1, 1), aabb.yMax() * m(1, 1));
  addMinMax(yMin, yMax, aabb.zMin() * m(2, 1), aabb.zMax() * m(2, 1));

  addMinMax(zMin, zMax, aabb.xMin() * m(0, 2), aabb.xMax() * m(0, 2));
  addMinMax(zMin, zMax, aabb.yMin() * m(1, 2), aabb.yMax() * m(1, 2));
  addMinMax(zMin, zMax, aabb.zMin() * m(2, 2), aabb.zMax() * m(2, 2));

  // translate
  xMin += m(4, 0);
  xMax += m(4, 0);

  yMin += m(4, 1);
  yMax += m(4, 1);

  zMin += m(4, 2);
  zMax += m(4, 2);

  return osg::BoundingBox(xMin, xMax, yMin, yMax, zMin, zMax);
}

//------------------------------------------------------------------------------
osg::Plane Math::getBestFitPlane(const osg::Vec3Array& vertices) {
  // TODO understand this
  osg::Vec3 n;
  double d = 0.0f;
  GLuint size = vertices.size();
  for (GLuint i = 0; i < size; ++i) {
    const osg::Vec3& v0 = vertices[i];
    const osg::Vec3& v1 = vertices[(i + 1) % size];
    n.x() += (v0[2] + v1[2]) * (v0[1] - v1[1]);
    n.y() += (v0[0] + v1[0]) * (v0[2] - v1[2]);
    n.z() += (v0[1] + v1[1]) * (v0[0] - v1[0]);
  }

  n.normalize();
  for (GLuint i = 0; i < size; ++i) {
    d -= n * vertices[i];
  }
  d /= size;

  return osg::Plane(n, d);
}

//------------------------------------------------------------------------------
osg::Vec3 Math::getBarycentric(const osg::Vec3& a, const osg::Vec3& b,
  const osg::Vec3& c, const osg::Vec3& p) {
  // use are property to get barientric of P

  osg::Vec3 ab = b - a;
  osg::Vec3 ac = c - a;
  osg::Vec3 bc = c - b;
  osg::Vec3 ca = -ac;
  osg::Vec3 ap = p - a;
  osg::Vec3 bp = p - b;
  osg::Vec3 cp = p - c;

  osg::Vec3 n = ab ^ ac;
  double abc = n.normalize();

  if (abc == 0) {
    throw std::runtime_error("invalid triangle");
  }

  double abp = (ab ^ ap) * n;
  double bcp = (bc ^ bp) * n;
  double cap = (ca ^ cp) * n;

  return osg::Vec3(bcp / abc, cap / abc, abp / abc);
}

//------------------------------------------------------------------------------
Circle Math::getInscribeCircle(
  const osg::Vec3& a, const osg::Vec3& b, const osg::Vec3& c) {
  Circle circle;

  osg::Vec3 ab = b - a;
  osg::Vec3 ac = c - a;
  osg::Vec3 bc = c - b;

  // barycentric of incenter is (lbc/p, lac/p, lab/p)
  double lab = ab.length();
  double lac = ac.length();
  double lbc = bc.length();

  double perimeter = lab + lac + lbc;
  circle.center = (a * lbc + b * lac + c * lab) / perimeter;
  double area = (ab ^ ac).length();
  circle.radius = area / perimeter;

  return circle;
}

//------------------------------------------------------------------------------
Circle Math::getCircumscribeCircle(
  const osg::Vec3& a, const osg::Vec3& b, const osg::Vec3& c) {
  // barcentric of circummcenter is (sin(2alpha) : sin02beta0 : sin(2gamma))
  // or sin(alpha)cos(alpha) : sin(alpha)cos(alpha) : sin(alpha)cos(alpha)

  Circle circle;

  double l2a = (b - c).length2();
  double l2b = (a - c).length2();
  double l2c = (a - b).length2();
  // baricentric of circumcenter
  double b0 = l2a * (l2b + l2c - l2a);
  double b1 = l2b * (l2a + l2c - l2b);
  double b2 = l2c * (l2a + l2b - l2c);
  // normalize it
  double f = 1 / (b0 + b1 + b2);
  b0 *= f;
  b1 *= f;
  b2 *= f;

  circle.center = a * b0 + b * b1 + c * b2;
  circle.radius = (circle.center - a).length();

  return circle;
}

//------------------------------------------------------------------------------
bool Math::colinear(
  const osg::Vec3& a, const osg::Vec3& b, const osg::Vec3& c) {
  return isAboutd((a - b) ^ (a - c), osg::Vec3());
}

//------------------------------------------------------------------------------
bool Math::isConvex(const osg::Vec3Array& vertices) {
  // sum of all smaller angle between tow edges must be (n-2)*180.
  // concave will break this rule

  double total = 0;
  GLuint size = vertices.size();

  if (size < 3)
    throw std::runtime_error("polygon must has at least 3 vertices");
  for (GLuint i = 0; i < size; ++i) {
    const osg::Vec3& v0 = vertices[i];                      // corner vertex
    const osg::Vec3& v1 = vertices[(i + 1) % size];         // next vertex
    const osg::Vec3& v2 = vertices[(i + size - 1) % size];  // previous vertex
    osg::Vec3 e0 = v1 - v0;
    osg::Vec3 e1 = v2 - v0;
    total += angle(e0, e1);
  }

  return isAboutd(total, (size - 2) * osg::PI);
}
}
