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
std::ostream& operator<<(std::ostream& os, const Vec3Vector& v) {
  auto size = v.size();
  for (size_t i = 0; i < size; ++i) {
    os << v[i] << std::endl;
  }
  return os;
}

//--------------------------------------------------------------------
glm::mat4 arcball(const glm::vec2& p0, const glm::vec2& p1,
  const glm::mat4& windowMatrixInverse, GLfloat radius /* = 0.8*/) {
  glm::vec4 n0 = windowMatrixInverse * glm::vec4(p0, 0, 1);
  glm::vec4 n1 = windowMatrixInverse * glm::vec4(p1, 0, 1);
  glm::vec3 sp0 = ndcToSphere(n0.xy(), radius);
  glm::vec3 sp1 = ndcToSphere(n1.xy(), radius);

  GLfloat rpRadius = 1 / radius;

  // get rotate axis in camera space
  glm::vec3 axis = cross(sp0, sp1);

  GLfloat theta = glm::orientedAngle(sp0 * rpRadius, sp1 * rpRadius, axis);


  return glm::rotate(theta, axis);
}

//--------------------------------------------------------------------
glm::vec3 ndcToSphere(const glm::vec2& p, GLfloat radius /* = 0.8f*/) {
  GLfloat l2 = glm::length2(p);
  if (l2 >= radius * radius) {
    return glm::vec3(p * radius / glm::sqrt(l2), 0);
  } else {
    return glm::vec3(p, glm::sqrt(radius * radius - l2));
  }
}

//--------------------------------------------------------------------
glm::mat4 computeWindowMatrix(
  GLint x, GLint y, GLint width, GLint height, GLfloat n, GLfloat f) {
  glm::mat4 m;

  // m = glm::translate(vec3(x, y, 0)) * glm::scale(vec3(width, height, 1)) +
  // glm::translate(vec3(0.5)) * glm::scale(vec3(0.5));

  m[0][0] = width / 2.0;
  m[1][1] = height / 2.0;
  m[2][2] = (f - n) / 2.0;
  m[3][0] = x + m[0][0];
  m[3][1] = y + m[1][1];
  m[3][2] = (f + n) / 2.0;

  return m;
}

//--------------------------------------------------------------------
glm::mat4 computeWindowMatrixInverse(
  GLint x, GLint y, GLint width, GLint height, GLfloat n, GLfloat f) {
  glm::mat4 m;
  m[0][0] = 2.0 / width;
  m[1][1] = 2.0 / height;
  m[2][2] = 2.0 / (f - n);
  m[3][0] = -1 - 2 * x / width;
  m[3][1] = -1 - 2 * y / height;
  m[3][2] = -(f + n) / (f - n);

  return m;
}
}
