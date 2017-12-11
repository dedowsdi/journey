#include "glm.h"
#include <iomanip>
using namespace glm;

namespace zxd {

//--------------------------------------------------------------------
mat4 getTangetnBasis(const vec3& v0, const vec3& v1, const vec3& v2,
  const vec2& texcoord0, const vec2& texcoord1, const vec2& texcoord2,
  const vec3* normal /* = 0*/) {
  vec3 dtPos01 = v1 - v0;
  vec3 dtPos02 = v2 - v0;
  vec2 dtTex01 = texcoord1 - texcoord0;
  vec2 dtTex02 = texcoord2 - texcoord0;

  float r = 1.0 / (dtTex01.x * dtTex02.y - dtTex02.x * dtTex01.y);
  vec3 tangent = (dtPos01 * dtTex02.y - dtPos02 * dtTex01.y) * r;
  tangent = glm::normalize(tangent);
  vec3 bitangent = (dtPos02 * dtTex01.x - dtPos01 * dtTex02.x) * r;
  bitangent = glm::normalize(bitangent);
  return createMat4(
    tangent, bitangent, normal == 0 ? getFaceNormal(v0, v1, v2) : *normal);
}

//------------------------------------------------------------------------------
bool operator<(const vec3& lhs, const vec3& rhs) {
  return lhs.x < rhs.x && lhs.y < rhs.y && lhs.z < rhs.z;
}

//------------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& os, const vec2& v) {
  static GLuint w = 12;
  static GLuint p = 6;
  os << std::left << std::fixed;
  // os << std::left;
  os << std::setw(w) << std::setprecision(p) << v.x   // x
     << std::setw(w) << std::setprecision(p) << v.y;  // y
  return os;
}

//------------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& os, const vec3& v) {
  static GLuint w = 12;
  static GLuint p = 6;
  os << std::left << std::fixed;
  // os << std::left;
  os << std::setw(w) << std::setprecision(p) << v.x   // x
     << std::setw(w) << std::setprecision(p) << v.y   // y
     << std::setw(w) << std::setprecision(p) << v.z;  // z
  return os;
}

//------------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& os, const vec4& v) {
  static GLuint w = 12;
  static GLuint p = 6;
  os << std::left << std::fixed;
  os << std::setw(w) << std::setprecision(p) << v.x  // x
     << std::setw(w) << std::setprecision(p) << v.y  // y
     << std::setw(w) << std::setprecision(p) << v.z  // z
     << std::setw(w) << std::setprecision(p) << v.w;
  return os;
}

//------------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& os, const mat2& m) {
  for (int i = 0; i < 2; ++i) {
    os << row(m, i) << std::endl;
  }
  return os;
}

//------------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& os, const mat3& m) {
  for (int i = 0; i < 3; ++i) {
    os << row(m, i) << std::endl;
  }
  return os;
}

//------------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& os, const mat4& m) {
  for (int i = 0; i < 4; ++i) {
    os << row(m, i) << std::endl;
  }
  return os;
}

//------------------------------------------------------------------------------
std::ostream& operator<<(std::ostream& os, const Vec3Vec& v) {
  auto size = v.size();
  for (size_t i = 0; i < size; ++i) {
    os << v[i] << std::endl;
  }
  return os;
}
}
