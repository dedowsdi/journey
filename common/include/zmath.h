#ifndef ZMATH_H
#define ZMATH_H

#include <osg/Matrix>
#include <osg/Vec2>
#include <utility>
#include <osg/Plane>

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
  static osg::Matrix getMatRS(const osg::Matrix& m) {
    osg::Matrix nm(m);
    nm(3, 0) = nm(3, 1) = nm(3, 2) = 0;
    return nm;
  }

  static osg::Matrix getMatR(const osg::Matrix& m) {
    osg::Vec3 translation;
    osg::Quat rotation;
    osg::Vec3 scale;
    osg::Quat so;
    m.decompose(translation, rotation, scale, so);
    return osg::Matrix(rotation);
  }

  // return matrix with only translation
  static osg::Matrix getMatT(const osg::Matrix& m) {
    osg::Matrix nm;
    nm(3, 0) = m(3, 0);
    nm(3, 1) = m(3, 1);
    nm(3, 2) = m(3, 2);
    return nm;
  }
};
}

#endif /* ZMATH_H */
