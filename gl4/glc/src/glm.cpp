#include "glm.h"

#include <iomanip>

#include <glm/gtc/matrix_access.hpp>

namespace zxd
{

//------------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& os, const vec2& v)
{
  static GLuint w = 12;
  static GLuint p = 6;
  os << std::left << std::fixed;
  // os << std::left;
  os << std::setw(w) << std::setprecision(p) << v.x << " "   // x
     << std::setw(w) << std::setprecision(p) << v.y;  // y
  return os;
}

//------------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& os, const vec3& v)
{
  static GLuint w = 12;
  static GLuint p = 6;
  os << std::left << std::fixed;
  // os << std::left;
  os << std::setw(w) << std::setprecision(p) << v.x << " "   // x
     << std::setw(w) << std::setprecision(p) << v.y << " "   // y
     << std::setw(w) << std::setprecision(p) << v.z;  // z
  return os;
}

//------------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& os, const vec4& v)
{
  static GLuint w = 12;
  static GLuint p = 6;
  os << std::left << std::fixed;
  os << std::setw(w) << std::setprecision(p) << v.x << " "  // x
     << std::setw(w) << std::setprecision(p) << v.y << " "  // y
     << std::setw(w) << std::setprecision(p) << v.z << " "  // z
     << std::setw(w) << std::setprecision(p) << v.w;
  return os;
}

//------------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& os, const mat2& m)
{
  for (int i = 0; i < 2; ++i)
  {
    os <<  row(m,i) << std::endl;
  }
  return os;
}

//------------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& os, const mat3& m)
{
  for (int i = 0; i < 3; ++i)
  {
    os <<  row(m,i) << std::endl;
  }
  return os;
}

//------------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& os, const mat4& m)
{
  for (int i = 0; i < 4; ++i)
  {
    os <<  row(m,i) << std::endl;
  }
  return os;
}

//------------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& os, const zxd::vec3_vector& v)
{
  auto size = v.size();
  for (size_t i = 0; i < size; ++i)
  {
    os <<  v[i] << std::endl;
  }
  return os;
}

}
