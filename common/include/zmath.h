#ifndef ZMATH_H
#define ZMATH_H

#include <osg/Matrix>
#include <osg/Vec2>
#include <utility>
#include <osg/Plane>
#include <algorithm>
#include <osg/Camera>

namespace zxd {

typedef std::pair<int, std::pair<osg::Vec3, osg::Vec3>> LineRelPair;
typedef std::pair<int, osg::Vec3> LinePlaneRelPair;

struct RayRel {
  int type;  // 0 parallel, 1 intersect, 2 skew
  float t0;  // v0 + dir0 * t0 = sk0
  float t1;  // v1 + dir1 * t1 = sk1
  osg::Vec3 sk0;
  osg::Vec3 sk1;
};

struct RayPlaneRel {
  int type;  // 0 parallel, 1 intersect
  float t;   // v + dir * t = ip
  osg::Vec3 ip;
};

class Math {
public:
  static RayRel getLineRelation(const osg::Vec3& p0, const osg::Vec3& d0,
    const osg::Vec3& p1, const osg::Vec3& d1);

  static RayPlaneRel getLinePlaneRelation(
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
  inline osg::Matrix scaleAlong(const osg::Vec3& v, float k) {
    float k1 = k - 1;
    return osg::Matrix(
      1 + k1 * v[0] * v[0], k1 * v[0] * v[1], k1 * v[0] * v[2], 0,  // 0
      k1 * v[0] * v[1], 1 + k1 * v[1] * v[1], k1 * v[1] * v[2], 0,  // 1
      k1 * v[0] * v[2], k1 * v[1] * v[2], 1 + k1 * v[2] * v[2], 0,  // 2
      0, 0, 0, 1                                                    // 3
      );
  }

  // scale along arbitary plane normal, the d part doesn't matter
  inline osg::Matrix scaleAlongPlane(const osg::Vec3& n, float k) {
    float k1 = 1 - k;
    return osg::Matrix(
      k + k1 * n[0] * n[0], k1 * n[0] * n[1], k1 * n[0] * n[2], 0,  // 0
      k1 * n[0] * n[1], k + k1 * n[1] * n[1], k1 * n[1] * n[2], 0,  // 1
      k1 * n[0] * n[2], k1 * n[1] * n[2], k + k1 * n[2] * n[2], 0,  // 2
      0, 0, 0, 1                                                    // 3
      );
  }

  // project on plane that pass origin with normal v
  inline osg::Matrix projectAlong(const osg::Vec3& v) {
    return scaleAlong(v, 0);
  }

  // reflect along arbitary vector
  inline osg::Matrix reflectAlong(const osg::Vec3& v) {
    return scaleAlong(v, -1);
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
};
}

#endif /* ZMATH_H */
