#ifndef GEOMETRY_UTIL_H
#define GEOMETRY_UTIL_H

namespace zxd
{
  class geometry_base;
}

namespace geometry_util
{
  // smooth_old of osg
  void smooth(zxd::geometry_base& gb, unsigned normal_attrib_index = 1);
}


#endif /* GEOMETRY_UTIL_H */
