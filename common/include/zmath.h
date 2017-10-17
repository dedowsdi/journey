#ifndef ZMATH_H
#define ZMATH_H

#include <osg/Matrix>
#include <osg/Vec2>
#include <utility>
#include <osg/Plane>
#include <algorithm>
#include <osg/Camera>
#include <osg/BoundingBox>
#include "osg/BoundingSphere"

namespace zxd {

struct LineRel {
  int type;  // 0 parallel, 1 intersect, 2 skew
  float t0;  // v0 + dir0 * t0 = sk0
  float t1;  // v1 + dir1 * t1 = sk1
  osg::Vec3 sk0;
  osg::Vec3 sk1;
};

struct LinePlaneRel {
  int type;  // 0 parallel, 1 intersect
  float t;   // v + dir * t = ip
  osg::Vec3 ip;
};

struct LineShpereRel {
  int type;  // 0 not intersect, 1 tangent, 2 intersect
  double t0;
  double t1;
  osg::Vec3 ip0;
  osg::Vec3 ip1;
};

struct Circle {
  double radius;
  osg::Vec3 center;
};

class Math {
public:
  static inline osg::Vec3f vec3fMull(
    const osg::Vec3f& v0, const osg::Vec3f& v1) {
    return osg::Vec3f(v0[0] * v1[0], v0[1] * v1[1], v0[2] * v1[2]);
  }
  static inline osg::Vec3d vec3dMull(
    const osg::Vec3d& v0, const osg::Vec3d& v1) {
    return osg::Vec3d(v0[0] * v1[0], v0[1] * v1[1], v0[2] * v1[2]);
  }

  static LineRel getLineRelation(const osg::Vec3& p0, const osg::Vec3& d0,
    const osg::Vec3& p1, const osg::Vec3& d1);

  static float getLineDistance(const osg::Vec3& p0, const osg::Vec3& d0,
    const osg::Vec3& p1, const osg::Vec3& d1);

  static inline float getLinePointDistance(
    const osg::Vec3& p0, const osg::Vec3& d0, const osg::Vec3& p1) {
    osg::Vec3 v01 = p1 - p0;
    return (v01 - d0 * (v01 * d0)).length();
  }

  static LinePlaneRel getLinePlaneRelation(
    const osg::Vec3& p0, const osg::Vec3& d0, const osg::Vec4& plane);

  // return matrix without translation
  static inline osg::Matrix getMatRS(const osg::Matrix& m) {
    osg::Matrix nm(m);
    nm(3, 0) = nm(3, 1) = nm(3, 2) = 0;
    return nm;
  }

  static inline osg::Matrix getMatR(const osg::Matrix& m) {
    osg::Vec3 translation;
    osg::Quat rotation;
    osg::Vec3 scale;
    osg::Quat so;
    m.decompose(translation, rotation, scale, so);
    return osg::Matrix(rotation);
  }

  // return matrix with only translation
  static inline osg::Matrix getMatT(const osg::Matrix& m) {
    osg::Matrix nm;
    nm(3, 0) = m(3, 0);
    nm(3, 1) = m(3, 1);
    nm(3, 2) = m(3, 2);
    return nm;
  }

  // get multiple element sin matrix, it's sequence is scale rotate translation
  // trso : translation rotation scale so
  static inline osg::Matrix getMatE(const osg::Matrix& m, std::string trso) {
    osg::Vec3 translation;
    osg::Quat rotation;
    osg::Vec3 scale;
    osg::Quat so;
    m.decompose(translation, rotation, scale, so);

    osg::Matrix res;

    if (trso.find_first_of('s') != std::string::npos)
      res *= osg::Matrix::scale(scale);
    if (trso.find_first_of('r') != std::string::npos)
      res *= osg::Matrix::rotate(rotation);
    if (trso.find_first_of('o') != std::string::npos)
      res *= osg::Matrix::rotate(so);
    if (trso.find_first_of('t') != std::string::npos)
      res *= osg::Matrix::translate(translation);

    return res;
  }

  static inline float angle(const osg::Vec3& v0, const osg::Vec3& v1) {
    return acos(v0 * v1 / (v0.length() * v1.length()));
  }

  // [0,pi/2]
  static inline float angleLine(const osg::Vec3& v0, const osg::Vec3& v1) {
    float vecAngle = angle(v0, v1);
    return vecAngle > osg::PI_2 ? osg::PI - vecAngle : vecAngle;
  }

  // get angle which denote rotate alone rotAxis from  v0 to v1,
  static inline float angleSigned(
    const osg::Vec3& v0, const osg::Vec3& v1, const osg::Vec3& rotAxis) {
    float a = angle(v0, v1);
    return (v0 ^ v1) * rotAxis >= 0 ? a : -a;
  }

  inline void copyRS(const osg::Matrix& from, osg::Matrix& to) {
    to(0, 0) = from(0, 0);
    to(0, 1) = from(0, 1);
    to(0, 2) = from(0, 2);

    to(1, 0) = from(1, 0);
    to(1, 1) = from(1, 1);
    to(1, 2) = from(1, 2);

    to(2, 0) = from(2, 0);
    to(2, 1) = from(2, 1);
    to(2, 2) = from(2, 2);
  }

  // scale along arbitary vector
  inline osg::Matrix scaleAlongVector(const osg::Vec3& v, float k) {
    float k1 = k - 1;
    return osg::Matrix(
      1 + k1 * v[0] * v[0], k1 * v[0] * v[1], k1 * v[0] * v[2], 0,  // 0
      k1 * v[0] * v[1], 1 + k1 * v[1] * v[1], k1 * v[1] * v[2], 0,  // 1
      k1 * v[0] * v[2], k1 * v[1] * v[2], 1 + k1 * v[2] * v[2], 0,  // 2
      0, 0, 0, 1                                                    // 3
      );
  }

  // scale along arbitary plane, the d part doesn't matter
  inline osg::Matrix scaleAlongPlane(const osg::Vec3& n, float k) {
    float k1 = 1 - k;
    return osg::Matrix(
      k + k1 * n[0] * n[0], k1 * n[0] * n[1], k1 * n[0] * n[2], 0,  // 0
      k1 * n[0] * n[1], k + k1 * n[1] * n[1], k1 * n[1] * n[2], 0,  // 1
      k1 * n[0] * n[2], k1 * n[1] * n[2], k + k1 * n[2] * n[2], 0,  // 2
      0, 0, 0, 1                                                    // 3
      );
  }

  // normal must be normalized
  osg::Vec3 projectPointOnPlane(const osg::Plane& plane, osg::Vec3 p) {
    return p - plane.getNormal() * plane.distance(p);
  }
  // dir must be normalized
  osg::Vec3 projectPointOnLine(
    const osg::Vec3& origin, const osg::Vec3& dir, const osg::Vec3& p) {
    double t = (p - origin) * dir;
    return origin + dir * t;
  }

  // as close as possible
  osg::Vec3 projectPointOnRay(const osg::Vec3& origin, const osg::Vec3& dir,
    double maxT, const osg::Vec3& p) {
    double t = (p - origin) * dir;
    t = fmax(fmin(maxT, t), 0);
    return origin + dir * t;
  }

  // always return point on sphere, no matter p is in or out of phere.
  // if p concidie with origin, return point on +x
  osg::Vec3 projectPointOnSphere(
    const osg::BoundingSphere& sphere, const osg::Vec3& p) {
    osg::Vec3 pointToCenter = sphere.center() - p;
    double l = 0;

    // check if point overlap with origin
    if (pointToCenter == osg::Vec3()) {
      pointToCenter = -osg::X_AXIS;
    } else {
      l = pointToCenter.normalize();
    }
    return p + pointToCenter * (l - sphere.radius());
  }

  // closest point on aabb to p
  osg::Vec3 projectPointOnAABB(const osg::BoundingBox& bb, const osg::Vec3& p) {
    osg::Vec3 v;
    v[0] = p[0] <= bb.xMin() ? bb.xMin() : bb.xMax();
    v[1] = p[1] <= bb.yMin() ? bb.yMin() : bb.yMax();
    v[2] = p[2] <= bb.zMin() ? bb.zMin() : bb.zMax();
    return v;
  }

  // project on arbitary plane, plane normal must be normalized
  inline osg::Matrix projectOnPlane(const osg::Plane& p) {
    return scaleAlongVector(p.getNormal(), 0) *
           osg::Matrix::translate(p.getNormal() * -p[3]);
  }

  // reflect along arbitary plane, plane normal must be normalized
  inline osg::Matrix reflectOnPlane(const osg::Plane& p) {
    return scaleAlongVector(p.getNormal(), -1) *
           osg::Matrix::translate(p.getNormal() * (p[3] * 2));
  }

  // Gram-Schmidt
  static osg::Matrix orthogonalizBiased(
    const osg::Matrix& m, uint startIndex = 0);
  // recursive
  static osg::Matrix orthogonalizIterate(
    const osg::Matrix& m, uint startIndex = 0, uint times = 4, float f = 0.25f);

  template <typename T>
  static inline T linearInterpolate(const T& t0, const T& t1, float f) {
    return t0 * (1 - f) + t1 * f;
  }

  static inline bool isAboutf(
    const float& t0, const float& t1, float epsilon = 0.0001f) {
    return std::abs(t0 - t1) <= epsilon;
  }
  static inline bool isAboutd(
    const double& t0, const double& t1, double epsilon = 0.0001f) {
    return std::abs(t0 - t1) <= epsilon;
  }
  static inline bool isAboutf(
    const osg::Vec3& t0, const osg::Vec3& t1, float epsilon = 0.0001f) {
    return isAboutf(t0[0], t0[1], epsilon) && isAboutf(t0[0], t0[1], epsilon) &&
           isAboutf(t0[0], t0[1], epsilon);
  }
  static inline bool isAboutd(
    const osg::Vec3& t0, const osg::Vec3& t1, float epsilon = 0.0001f) {
    return isAboutd(t0[0], t0[1], epsilon) && isAboutd(t0[0], t0[1], epsilon) &&
           isAboutd(t0[0], t0[1], epsilon);
  }

  // wrap v between 2pi - positiveLimit and positiveLimit
  static inline double clampAngle(
    double v, double minValue = 0.0f, double maxValue = osg::PI * 2) {
    v = fmod(v, osg::PI * 2);
    if (v < minValue)
      v += osg::PI * 2;
    else if (v > maxValue)
      v -= osg::PI * 2;

    return v;
  }

  /*
   * let cut plane which contains (0,0,-distance) be the focus plane.
   * (intersection plane between persp and ortho)
   */
  static bool perspToOrtho(
    const osg::Matrix& m, double distance, osg::Matrix& om) {
    double left, right, bottom, top, near, far;
    if (!m.getFrustum(left, right, bottom, top, near, far)) return false;

    // get symettric top, aspectRatio
    double smTop = (std::abs(bottom) + std::abs(top)) / 2.0f;
    double smRight = (std::abs(left) + std::abs(right)) / 2.0f;
    double aspectRatio = smRight / smTop;

    // scale to focus plane
    double distanceScale = distance / near;

    // center offset
    double offsetX = distanceScale * (left + right) / 2.0f;
    double offsetY = distanceScale * (bottom + top) / 2.0f;

    // get symettric ortho l r t b
    double orthoTop = distanceScale * smTop;
    double orthoBottom = -orthoTop;
    double orthoRight = orthoTop * aspectRatio;
    double orthoLeft = -orthoRight;

    // apply offset
    orthoTop += offsetY;
    orthoBottom += offsetY;
    orthoLeft += offsetX;
    orthoRight += offsetX;

    om.makeOrtho(orthoLeft, orthoRight, orthoBottom, orthoTop, near, far);

    return true;
  }

  /*
   * reverse of perspToOrtho, except use fovy instead of distance, distance
   * always changes, but fovy is kuai stable
   */
  static bool orthoToPersp(const osg::Matrix& m, double fovy, osg::Matrix& om) {
    double left, right, bottom, top, near, far;
    if (!m.getOrtho(left, right, bottom, top, near, far)) return false;

    // get symettric top, aspectRatio
    double smTop = (std::abs(bottom) + std::abs(top)) / 2.0f;
    double smRight = (std::abs(left) + std::abs(right)) / 2.0f;
    double aspectRatio = smRight / smTop;

    // scale to near plane of persp
    // double distanceScale =
    // near / (smTop / std::tan(osg::DegreesToRadians(fovy * 0.5f)));
    double distanceScale =
      near * std::tan(osg::DegreesToRadians(fovy * 0.5f)) / smTop;

    // center offset
    double offsetX = distanceScale * (left + right) / 2.0f;
    double offsetY = distanceScale * (bottom + top) / 2.0f;

    // get symettric ortho l r t b
    double perspTop = distanceScale * smTop;
    double perspBottom = -perspTop;
    double perspRight = perspTop * aspectRatio;
    double perspLeft = -perspRight;

    // apply offset
    perspTop += offsetY;
    perspBottom += offsetY;
    perspLeft += offsetX;
    perspRight += offsetX;

    om.makeFrustum(perspLeft, perspRight, perspBottom, perspTop, near, far);

    return true;
  }

  static inline bool isOrthoProj(const osg::Matrix& m) {
    double left, right, bottom, top, near, far;
    return m.getOrtho(left, right, bottom, top, near, far);
  }

  static inline void getCameraRay(const osg::Vec2& cursor, const osg::Matrix& m,
    osg::Vec3& p0, osg::Vec3& p1) {
    p0 = osg::Vec3(cursor, 0);
    p1 = osg::Vec3(cursor, 1);
    p0 = p0 * m;
    p1 = p1 * m;
  }

  static inline void getCameraRay(const osg::Vec2& cursor, osg::Camera* camera,
    osg::Vec3& p0, osg::Vec3& p1) {
    osg::Matrix m = camera->getViewMatrix() * camera->getProjectionMatrix() *
                    camera->getViewport()->computeWindowMatrix();
    m.invert(m);
    getCameraRay(cursor, m, p0, p1);
  }

  inline osg::Matrix transpose(const osg::Matrix& m) {
    return osg::Matrix(                    //
      m(0, 0), m(1, 0), m(2, 0), m(3, 0),  // 0
      m(0, 1), m(1, 1), m(2, 1), m(3, 1),  // 1
      m(0, 2), m(1, 2), m(2, 2), m(3, 2),  // 2
      m(0, 3), m(1, 3), m(2, 3), m(3, 3)   // 3
      );
  }

  // becareful this returns intrinsic rotation order.
  // M = M(z)M(y)M(x)
  // M = matrix.makerotate(z, y, x)
  // 0 <= x <= 2pi
  // 0 <= y <= pi/2 || 3pi/2 <= y <= 2pi
  // 0 <= z <= 2pi
  static osg::Vec3 getEulerXYZ(const osg::Matrixf& m);

  static inline float randomValue(float min, float max) {
    return (min + (float)rand() / (RAND_MAX + 1.0f) * (max - min));
  }

  static inline osg::Vec3 randomVector(float min, float max) {
    return osg::Vec3(
      randomValue(min, max), randomValue(min, max), randomValue(min, max));
  }
  static inline osg::Vec4 randomVector4(float min, float max) {
    return osg::Vec4(randomValue(min, max), randomValue(min, max),
      randomValue(min, max), randomValue(min, max));
  }

  static inline osg::Matrix randomMatrix(float min, float max) {
    osg::Vec3 rot = randomVector(-osg::PI, osg::PI);
    osg::Vec3 pos = randomVector(min, max);
    return osg::Matrix::rotate(
             rot[0], osg::X_AXIS, rot[1], osg::Y_AXIS, rot[2], osg::Z_AXIS) *
           osg::Matrix::translate(pos);
  }

  static osg::Matrix arcball(
    const osg::Vec2& np0, const osg::Vec2& np1, GLfloat radius = 0.8);
  static osg::Vec2 screenToNdc(GLfloat x, GLfloat y, GLfloat cx, GLfloat cy);
  static osg::Vec2 screenToNdc(GLfloat nx, GLfloat ny);
  static osg::Vec3 ndcToSphere(const osg::Vec2& np0, GLfloat radius = 0.9f);

  // upper corner times n must be new max, lower corner times n must be new min
  static inline void getBBCorner(
    const osg::Vec3& n, GLuint& upper, GLuint& lower) {
    upper = (n[0] >= 0 ? 1 : 0) | (n[1] >= 0 ? 2 : 0) * (n[2] >= 0 ? 4 : 0);
    lower = (~upper) & 7;
    getBBCorner(n[0], n[1], n[2], upper, lower);
  }

  static inline void getBBCorner(
    double x, double y, double z, GLuint& upper, GLuint& lower) {
    upper = (x >= 0 ? 1 : 0) | (y >= 0 ? 2 : 0) * (z >= 0 ? 4 : 0);
    lower = (~upper) & 7;
  }

  // xMin += min(a,b) xMax+= max(a,b)
  static inline void addMinMax(double& xMin, double& xMax, double a, double b) {
    if (a >= b) {
      xMin += b;
      xMax += a;
    } else {
      xMin += a;
      xMax += b;
    }
  }

  static osg::BoundingBox transformAABB(
    const osg::BoundingBox& aabb, const osg::Matrix& m);

  static osg::Plane getBestFitPlane(const osg::Vec3Array& vertices);

  // interection point of 3 point
  static std::pair<bool, osg::Vec3> intersect(
    const osg::Plane& p0, const osg::Plane& p1, const osg::Plane& p2);

  // dir must be normalized
  LineShpereRel intersect(const osg::BoundingSphere& sphere, const osg::Vec3& p,
    const osg::Vec3& dir);

  // get triangle areay by Heron's formula
  static inline double heronArea(double l0, double l1, double l2) {
    double s = (l0 + l1 + l2) / 2;
    return std::sqrt(s * (s - l0) * (s - l1) * (s - l2));
  }

  static osg::Vec3 getBarycentric(const osg::Vec3& a, const osg::Vec3& b,
    const osg::Vec3& c, const osg::Vec3& p);

  static osg::Vec3 getCentroid(
    const osg::Vec3& a, const osg::Vec3& b, const osg::Vec3& c) {
    return (a + b + c) / 3;
  }

  static Circle getInscribeCircle(
    const osg::Vec3& a, const osg::Vec3& b, const osg::Vec3& c);

  static Circle getCircumscribeCircle(
    const osg::Vec3& a, const osg::Vec3& b, const osg::Vec3& c);

  static bool colinear(
    const osg::Vec3& a, const osg::Vec3& b, const osg::Vec3& c);

  static bool isConvex(const osg::Vec3Array& vertices);

  static uint factorial(uint n) {
    uint res = 1;
    while (n > 1) res *= n--;
    return res;
  }

  //doesn't work if n or k is too big
  static uint binomial(uint n, uint k) {
    if (k == 0 || k == n) return 1;

    // make sure k < n/2
    if (k > n / 2) k = n - k;

    uint no = 1;
    uint den = 1;

    while (k > 0) {
      no *= n--;
      den *= k--;
    }
    return no / den;
  }

  static inline float sigma(float a0, float d, uint n) {
    return (a0 + (n - 1) * d / 2) * n;
  }
  static inline float pi(float a0, float d, uint n) {
    float m = 1;
    while (n--) m *= a0 + n * d;  // reverse order of pi
    return m;
  }
};
}

#endif /* ZMATH_H */
