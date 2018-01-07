#ifndef GLM_H
#define GLM_H
#define GLM_SWIZZLE
#include "glad/glad.h"
#include <glm/glm.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/random.hpp>

#include <glm/gtc/constants.hpp>
#include <glm/gtc/random.hpp>
#include <vector>
#include <iostream>
#include <utility>

namespace zxd {
using namespace glm;

typedef std::vector<mat2> Mat2Vector;
typedef std::vector<mat3> Mat3Vector;
typedef std::vector<mat4> Mat4Vector;
typedef std::vector<vec2> Vec2Vector;
typedef std::vector<vec3> Vec3Vector;
typedef std::vector<vec4> Vec4Vector;

// follow convention of glm, always return

inline vec3 makeFloor(const vec3& lhs, const vec3& rhs);

inline vec3 makeCeil(const vec3& lhs, const vec3& rhs);

inline vec3 axis(const mat4& m, GLuint i) { return vec3(column(m, i)); }

inline mat4 createMat4(const vec3& xAxis, const vec3& yAxis, const vec3& zAxis);

// radian between any vector, no need to normalize first
inline GLfloat angleAny(const vec3& lhs, const vec3& rhs);

// radian between any vector, no need to normalize first
inline GLfloat orientedAngleAny(
  const vec3& lhs, const vec3& rhs, const vec3& ref);

inline vec3 getFaceNormal(const vec3& v0, const vec3& v1, const vec3& v2);

mat4 getTangetnBasis(const vec3& v0, const vec3& v1, const vec3& v2,
  const vec2& texcoord0, const vec2& texcoord1, const vec2& texcoord2, const vec3* normal = 0);

// generate normals for triangles
template <typename _It, typename _OutIt>
inline void generateFaceNormals(_It beg, _It end, _OutIt out);

// assume w is
inline vec3 transformPosition(const mat4& m, const vec3& v);

// assume w is 0
inline vec3 transformVector(const mat4& m, const vec3& v);

// assume w is 1
inline vec3 operator*(const mat4& m, const vec3& v) { return transformPosition(m, v); }

bool operator<(const vec3& lhs, const vec3& rhs);

inline bool operator>(const vec3& lhs, const vec3& rhs);

std::ostream& operator<<(std::ostream& os, const vec2& v);
std::ostream& operator<<(std::ostream& os, const vec3& v);
std::ostream& operator<<(std::ostream& os, const vec4& v);
std::ostream& operator<<(std::ostream& os, const mat2& m);
std::ostream& operator<<(std::ostream& os, const mat3& m);
std::ostream& operator<<(std::ostream& os, const mat4& m);
std::ostream& operator<<(std::ostream& os, const Vec3Vector& v);

inline GLfloat maxAbsComponent(const vec3& v){
  GLfloat x = glm::abs(v[0]);
  GLfloat y = glm::abs(v[1]);
  GLfloat z = glm::abs(v[2]);
  return glm::max(glm::max(x, y), z);
}

//--------------------------------------------------------------------
inline vec3 makeFloor(const vec3& lhs, const vec3& rhs) {
  return vec3(
    std::min(lhs.x, rhs.x), std::min(lhs.y, rhs.y), std::min(lhs.z, rhs.z));
}

//--------------------------------------------------------------------
inline vec3 makeCeil(const vec3& lhs, const vec3& rhs) {
  return vec3(
    std::max(lhs.x, rhs.x), std::max(lhs.y, rhs.y), std::max(lhs.z, rhs.z));
}

//--------------------------------------------------------------------
inline mat4 createMat4(
  const vec3& xAxis, const vec3& yAxis, const vec3& zAxis) {
  return mat4(vec4(xAxis, 0), vec4(yAxis, 0), vec4(zAxis, 0),
    vec4(0.0f, 0.0f, 0.0f, 1.0f));
}

//--------------------------------------------------------------------
inline GLfloat angleAny(const vec3& lhs, const vec3& rhs) {
  return angle(normalize(lhs), normalize(rhs));
}

//--------------------------------------------------------------------
inline vec3 getFaceNormal(const vec3& v0, const vec3& v1, const vec3& v2) {
  vec3 v01 = v1 - v0;
  vec3 v02 = v2 - v0;
  vec3 normal = cross(v01, v02);
  return normalize(normal);
}

//--------------------------------------------------------------------
template <typename _It, typename _OutIt>
inline void generateFaceNormals(_It beg, _It end, _OutIt out) {
  typedef decltype(*beg) value_type;
  unsigned size = end - beg;
  if (size % 3 != 0) {
    throw std::runtime_error("number of vertices should be multiple of 3");
  }
  for (_It iter = beg; iter != end;) {
    value_type& v0 = *iter++;
    value_type& v1 = *iter++;
    value_type& v2 = *iter++;
    vec3 normal = getFaceNormal(v0, v1, v2);
    *out++ = normal;
    *out++ = normal;
    *out++ = normal;
  }
}
//--------------------------------------------------------------------
inline vec3 transformPosition(const mat4& m, const vec3& v) {
  vec3 r;

  GLfloat fInvW =
    1.0f / (m[0][3] * v.x + m[1][3] * v.y + m[2][3] * v.z + m[3][3]);
  r.x = (m[0][0] * v.x + m[1][0] * v.y + m[2][0] * v.z + m[3][0]) * fInvW;
  r.y = (m[0][1] * v.x + m[1][1] * v.y + m[2][1] * v.z + m[3][1]) * fInvW;
  r.z = (m[0][2] * v.x + m[1][2] * v.y + m[2][2] * v.z + m[3][2]) * fInvW;

  return r;
}

//--------------------------------------------------------------------
inline vec3 transformVector(const mat4& m, const vec3& v) {
  vec3 r;

  r.x = (m[0][0] * v.x + m[1][0] * v.y + m[2][0] * v.z);
  r.y = (m[0][1] * v.x + m[1][1] * v.y + m[2][1] * v.z);
  r.z = (m[0][2] * v.x + m[1][2] * v.y + m[2][2] * v.z);

  return r;
}

//--------------------------------------------------------------------
inline bool operator>(const vec3& lhs, const vec3& rhs) {
  return lhs != rhs && !operator<(lhs, rhs);
}
}

#endif /* GLM_H */
