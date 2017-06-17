#ifndef ZMATH_H
#define ZMATH_H

#include <osg/Vec3>
#include <osg/Vec2>
#include <utility>

namespace zxd {

typedef std::pair<int, std::pair<osg::Vec3, osg::Vec3>> LineRelPair;
typedef std::pair<int, osg::Vec3> LinePlaneRelPair;

class Math {
  /**
   * get relation of two lines
   * return:
   *   <0, <(0,0,0),(0,0,0)>> if parallel
   *   <1, (ip0,ip1)> if intersect
   *   <2, (sp0,sp1)> if skew
   */
  LineRelPair getLineRelation(const osg::Vec3& p0, const osg::Vec3& d0,
    const osg::Vec3& p1, const osg::Vec3& d1);

  /**
   * get relation of line and plane
   * @return :
   *   <0, (0,0,0)> if parallel
   *   <1, ip> if intersect
   */
  LinePlaneRelPair getLinePlaneRelation(
    const osg::Vec3& p0, const osg::Vec3& d0, const osg::Vec3& n, float d);
};
}

#endif /* ZMATH_H */
