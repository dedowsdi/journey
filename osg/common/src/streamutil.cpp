#include "streamutil.h"

//--------------------------------------------------------------------
std::ostream& operator<< (std::ostream& os, const osg::Vec2& v)
{
  os << v.x() << " " << v.y();
  return os;
}

//--------------------------------------------------------------------
std::ostream& operator<< (std::ostream& os, const osg::Vec3& v)
{
  os << v.x() << " " << v.y() << " " << v.z();
  return os;
}

//--------------------------------------------------------------------
std::ostream& operator<< (std::ostream& os, const osg::Vec4& v)
{
  os << v.x() << " " << v.y() << " " << v.z() << " " << v.w();
  return os;
}
