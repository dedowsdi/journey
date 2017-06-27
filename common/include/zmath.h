#ifndef ZMATH_H
#define ZMATH_H

#include <osg/Matrix>
#include <osg/Vec2>
#include <utility>
#include <osg/Plane>
#include <algorithm>

namespace zxd {

typedef std::pair<int, std::pair<osg::Vec3, osg::Vec3>> LineRelPair;
typedef std::pair<int, osg::Vec3> LinePlaneRelPair;

class Math {
public:
  /**
   * get relation of two lines
   * return:
   *   <0, <(0,0,0),(0,0,0)>> if parallel
   *   <1, (ip0,ip1)> if intersect
   *   <2, (sp0,sp1)> if skew
   */
  static LineRelPair getLineRelation(const osg::Vec3& p0, const osg::Vec3& d0,
    const osg::Vec3& p1, const osg::Vec3& d1);

  /**
   * get relation of line and plane
   * @return :
   *   <0, (0,0,0)> if parallel
   *   <1, ip> if intersect
   */
  static LinePlaneRelPair getLinePlaneRelation(
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

  static inline void copyRS(const osg::Matrix& from, osg::Matrix& to) {
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
  static inline osg::Matrix scaleAlong(const osg::Vec3& v, float k) {
    float k1 = k - 1;
    return osg::Matrix(
      1 + k1 * v[0] * v[0], k1 * v[0] * v[1], k1 * v[0] * v[2], 0,  // 0
      k1 * v[0] * v[1], 1 + k1 * v[1] * v[1], k1 * v[1] * v[2], 0,  // 1
      k1 * v[0] * v[2], k1 * v[1] * v[2], 1 + k1 * v[2] * v[2], 0,  // 2
      0, 0, 0, 1                                                    // 3
      );
  }

  // scale along arbitary plane normal, the d part doesn't matter
  static inline osg::Matrix scaleAlongPlane(const osg::Vec3& n, float k) {
    float k1 = 1 - k;
    return osg::Matrix(
      k + k1 * n[0] * n[0], k1 * n[0] * n[1], k1 * n[0] * n[2], 0,  // 0
      k1 * n[0] * n[1], k + k1 * n[1] * n[1], k1 * n[1] * n[2], 0,  // 1
      k1 * n[0] * n[2], k1 * n[1] * n[2], k + k1 * n[2] * n[2], 0,  // 2
      0, 0, 0, 1                                                    // 3
      );
  }

  // project on plane that pass origin with normal v
  static inline osg::Matrix projectAlong(const osg::Vec3& v) {
    return scaleAlong(v, 0);
  }

  // reflect along arbitary vector
  static inline osg::Matrix reflectAlong(const osg::Vec3& v) {
    return scaleAlong(v, -1);
  }

};
}

#endif /* ZMATH_H */
