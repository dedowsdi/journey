#include "glm.h"
#include <iomanip>
using namespace glm;

namespace zxd {

//--------------------------------------------------------------------
vec3 make_floor(const vec3& lhs, const vec3& rhs) {
  return vec3(
    std::min(lhs.x, rhs.x), std::min(lhs.y, rhs.y), std::min(lhs.z, rhs.z));
}

//--------------------------------------------------------------------
vec3 make_ceil(const vec3& lhs, const vec3& rhs) {
  return vec3(
    std::max(lhs.x, rhs.x), std::max(lhs.y, rhs.y), std::max(lhs.z, rhs.z));
}

//--------------------------------------------------------------------
vec3 axis(const mat4& m, GLuint i) { return vec3(column(m, i)); }

//--------------------------------------------------------------------
mat4 create_mat4(const vec3& xa, const vec3& ya, const vec3& za) {
  return mat4(
    vec4(xa, 0), vec4(ya, 0), vec4(za, 0), vec4(0.0f, 0.0f, 0.0f, 1.0f));
}

//--------------------------------------------------------------------
GLfloat angle_any(const vec3& lhs, const vec3& rhs) {
  return angle(normalize(lhs), normalize(rhs));
}

//--------------------------------------------------------------------
vec3 face_normal(const vec3& v0, const vec3& v1, const vec3& v2) {
  vec3 v01 = v1 - v0;
  vec3 v02 = v2 - v0;
  vec3 normal = cross(v01, v02);
  return normalize(normal);
}

//--------------------------------------------------------------------
mat4 get_tangetn_basis(const vec3& v0, const vec3& v1, const vec3& v2,
  const vec2& texcoord0, const vec2& texcoord1, const vec2& texcoord2,
  const vec3* normal /* = 0*/) {
  vec3 dt_pos01 = v1 - v0;
  vec3 dt_pos02 = v2 - v0;
  vec2 dt_tex01 = texcoord1 - texcoord0;
  vec2 dt_tex02 = texcoord2 - texcoord0;

  float r = 1.0 / (dt_tex01.x * dt_tex02.y - dt_tex02.x * dt_tex01.y);
  vec3 tangent = (dt_pos01 * dt_tex02.y - dt_pos02 * dt_tex01.y) * r;
  tangent = glm::normalize(tangent);
  vec3 bitangent = (dt_pos02 * dt_tex01.x - dt_pos01 * dt_tex02.x) * r;
  bitangent = glm::normalize(bitangent);
  return create_mat4(
    tangent, bitangent, normal == 0 ? face_normal(v0, v1, v2) : *normal);
}

//--------------------------------------------------------------------
vec3 transform_position(const mat4& m, const vec3& v) {
  vec3 r;

  GLfloat inv_w =
    1.0f / (m[0][3] * v.x + m[1][3] * v.y + m[2][3] * v.z + m[3][3]);
  r.x = (m[0][0] * v.x + m[1][0] * v.y + m[2][0] * v.z + m[3][0]) * inv_w;
  r.y = (m[0][1] * v.x + m[1][1] * v.y + m[2][1] * v.z + m[3][1]) * inv_w;
  r.z = (m[0][2] * v.x + m[1][2] * v.y + m[2][2] * v.z + m[3][2]) * inv_w;

  return r;
}

//--------------------------------------------------------------------
vec3 transform_vector(const mat4& m, const vec3& v) {
  vec3 r;

  r.x = (m[0][0] * v.x + m[1][0] * v.y + m[2][0] * v.z);
  r.y = (m[0][1] * v.x + m[1][1] * v.y + m[2][1] * v.z);
  r.z = (m[0][2] * v.x + m[1][2] * v.y + m[2][2] * v.z);

  return r;
}

//------------------------------------------------------------------------------
bool operator<(const vec3& lhs, const vec3& rhs) {
  return lhs.x < rhs.x && lhs.y < rhs.y && lhs.z < rhs.z;
}

//--------------------------------------------------------------------
bool operator>(const vec3& lhs, const vec3& rhs) {
  return lhs != rhs && !operator<(lhs, rhs);
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
std::ostream& operator<<(std::ostream& os, const vec3_vector& v) {
  auto size = v.size();
  for (size_t i = 0; i < size; ++i) {
    os << v[i] << std::endl;
  }
  return os;
}

//--------------------------------------------------------------------
GLfloat max_abs_component(const vec3& v) {
  GLfloat x = glm::abs(v[0]);
  GLfloat y = glm::abs(v[1]);
  GLfloat z = glm::abs(v[2]);
  return glm::max(glm::max(x, y), z);
}

//--------------------------------------------------------------------
glm::mat4 arcball(const glm::vec2& p0, const glm::vec2& p1,
  const glm::mat4& w_mat_i, GLfloat radius /* = 0.8*/) {
  glm::vec4 n0 = w_mat_i * glm::vec4(p0, 0, 1);
  glm::vec4 n1 = w_mat_i * glm::vec4(p1, 0, 1);
  glm::vec3 sp0 = ndc_tosphere(n0.xy(), radius);
  glm::vec3 sp1 = ndc_tosphere(n1.xy(), radius);

  GLfloat rp_radius = 1 / radius;

  // get rotate axis in camera space
  glm::vec3 axis = cross(sp0, sp1);

  GLfloat theta = glm::orientedAngle(sp0 * rp_radius, sp1 * rp_radius, axis);

  return glm::rotate(theta, axis);
}

//--------------------------------------------------------------------
glm::vec3 ndc_tosphere(const glm::vec2& p, GLfloat radius /* = 0.8f*/) {
  GLfloat l2 = glm::length2(p);
  if (l2 >= radius * radius) {
    return glm::vec3(p * radius / glm::sqrt(l2), 0);
  } else {
    return glm::vec3(p, glm::sqrt(radius * radius - l2));
  }
}

//--------------------------------------------------------------------
glm::mat4 compute_window_mat(
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
glm::mat4 compute_window_mat_i(
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

//--------------------------------------------------------------------
vec3 eye_pos(const mat4& v_mat) {
  const vec4& w2c = -v_mat[3];
  // clang-format off
	// project world to camera onto world x, y, z axis
  return vec3(
      w2c[0] * v_mat[0][0] + w2c[1] * v_mat[0][1] + w2c[2] * v_mat[0][2],
      w2c[0] * v_mat[1][0] + w2c[1] * v_mat[1][1] + w2c[2] * v_mat[1][2],
      w2c[0] * v_mat[2][0] + w2c[1] * v_mat[2][1] + w2c[2] * v_mat[2][2]
      );
  // clang-format on
}
}
