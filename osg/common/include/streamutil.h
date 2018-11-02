#ifndef STREAMUTIL_H
#define STREAMUTIL_H

#include <ostream>
#include <osg/Vec2>
#include <osg/Vec3>
#include <osg/Vec4>

std::ostream& operator<< (std::ostream& os, const osg::Vec2& v);
std::ostream& operator<< (std::ostream& os, const osg::Vec3& v);
std::ostream& operator<< (std::ostream& os, const osg::Vec4& v);

namespace streamUtil
{
  
}

#endif /* STREAMUTIL_H */
