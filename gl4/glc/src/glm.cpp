#include "glm.h"

#include <iomanip>

#include <glm/gtc/matrix_access.hpp>

namespace std
{

//------------------------------------------------------------------------------
ostream& operator<<(ostream& os, const vec2& v)
{
  static GLuint w = 12;
  static GLuint p = 6;
  os << left << fixed;
  // os << left;
  os << setw(w) << setprecision(p) << v.x   // x
     << setw(w) << setprecision(p) << v.y;  // y
  return os;
}

//------------------------------------------------------------------------------
ostream& operator<<(ostream& os, const vec3& v)
{
  static GLuint w = 12;
  static GLuint p = 6;
  os << left << fixed;
  // os << left;
  os << setw(w) << setprecision(p) << v.x   // x
     << setw(w) << setprecision(p) << v.y   // y
     << setw(w) << setprecision(p) << v.z;  // z
  return os;
}

//------------------------------------------------------------------------------
ostream& operator<<(ostream& os, const vec4& v)
{
  static GLuint w = 12;
  static GLuint p = 6;
  os << left << fixed;
  os << setw(w) << setprecision(p) << v.x  // x
     << setw(w) << setprecision(p) << v.y  // y
     << setw(w) << setprecision(p) << v.z  // z
     << setw(w) << setprecision(p) << v.w;
  return os;
}

//------------------------------------------------------------------------------
ostream& operator<<(ostream& os, const mat2& m)
{
  for (int i = 0; i < 2; ++i)
  {
    os <<  row(m,i) << endl;
  }
  return os;
}

//------------------------------------------------------------------------------
ostream& operator<<(ostream& os, const mat3& m)
{
  for (int i = 0; i < 3; ++i)
  {
    os <<  row(m,i) << endl;
  }
  return os;
}

//------------------------------------------------------------------------------
ostream& operator<<(ostream& os, const mat4& m)
{
  for (int i = 0; i < 4; ++i)
  {
    os <<  row(m,i) << endl;
  }
  return os;
}

//------------------------------------------------------------------------------
ostream& operator<<(ostream& os, const zxd::vec3_vector& v)
{
  auto size = v.size();
  for (size_t i = 0; i < size; ++i)
  {
    os <<  v[i] << endl;
  }
  return os;
}

}
