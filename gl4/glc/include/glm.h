#ifndef GLM_H
#define GLM_H
#define GLM_SWIZZLE
#define GLM_META_PROG_HELPERS

#include "gl.h"
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
#include <glm/gtc/noise.hpp>

#include <glm/gtc/constants.hpp>
#include <glm/gtc/random.hpp>
#include <vector>
#include <iostream>
#include <utility>

namespace zxd {
using namespace glm;

static const GLfloat fpi = glm::pi<GLfloat>();
static const GLfloat f2pi = fpi * 2;
static const GLfloat fpi2 = fpi / 2;
static const GLfloat fpi4 = fpi / 4;
static const GLdouble dpi = glm::pi<GLdouble>();
static const GLfloat d2pi = dpi * 2;
static const GLfloat dpi2 = dpi / 2;
static const GLfloat dpi4 = dpi / 4;

static const glm::vec3 pxa(1, 0, 0);
static const glm::vec3 nxa(-1, 0, 0);
static const glm::vec3 pya(0, 1, 0);
static const glm::vec3 nya(0, -1, 0);
static const glm::vec3 pza(0, 0, 1);
static const glm::vec3 nza(0, 0, -1);
static const glm::vec4 hpxa(1, 0, 0, 1);
static const glm::vec4 hnxa(-1, 0, 0, 1);
static const glm::vec4 hpya(0, 1, 0, 1);
static const glm::vec4 hnya(0, -1, 0, 1);
static const glm::vec4 hpza(0, 0, 1, 1);
static const glm::vec4 hnza(0, 0, -1, 1);
static const glm::vec3 zp(0, 0, 0);
static const glm::vec4 hzp(0, 0, 0, 1);

typedef std::vector<mat2> mat2_vector;
typedef std::vector<mat3> mat3_vector;
typedef std::vector<mat4> mat4_vector;
typedef std::vector<GLfloat> float_vector;
typedef std::vector<vec2> vec2_vector;
typedef std::vector<vec3> vec3_vector;
typedef std::vector<vec4> vec4_vector;

typedef std::vector<std::vector<GLfloat> > float_vector2;
typedef std::vector<std::vector<vec2> > vec2_vector2;
typedef std::vector<std::vector<vec3> > vec3_vector2;
typedef std::vector<std::vector<vec4> > vec4_vector2;

typedef float_vector::iterator fv_i;
typedef vec2_vector::iterator v2v_i;
typedef vec3_vector::iterator v3v_i;
typedef vec4_vector::iterator v4v_i;
typedef float_vector::const_iterator fv_ci;
typedef vec2_vector::const_iterator v2v_ci;
typedef vec3_vector::const_iterator v3v_ci;
typedef vec4_vector::const_iterator v4v_ci;


// follow convention of glm, always return

vec3 make_floor(const vec3& lhs, const vec3& rhs);

vec3 make_ceil(const vec3& lhs, const vec3& rhs);

vec3 axis(const mat4& m, GLuint i);

mat4 create_mat4(const vec3& xa, const vec3& ya, const vec3& za);

// radian between any vector, no need to normalize first
GLfloat angle_any(const vec3& lhs, const vec3& rhs);

// radian between any vector, no need to normalize first
GLfloat oriented_angle_any(const vec3& lhs, const vec3& rhs, const vec3& ref);

vec3 face_normal(const vec3& v0, const vec3& v1, const vec3& v2);

mat4 get_tangetn_basis(const vec3& v0, const vec3& v1, const vec3& v2,
  const vec2& texcoord0, const vec2& texcoord1, const vec2& texcoord2,
  const vec3* normal = 0);

// generate normals for triangles
template <typename _It, typename _OutIt>
void generate_face_normals(_It beg, _It end, _OutIt out);

// assume w is
vec3 transform_position(const mat4& m, const vec3& v);

// assume w is 0
vec3 transform_vector(const mat4& m, const vec3& v);

// v0, v1 must be normalized
mat4 rotate_to(const vec3& v0, const vec3& v1);

// assume w is 1
bool operator<(const vec3& lhs, const vec3& rhs);

bool operator>(const vec3& lhs, const vec3& rhs);

std::ostream& operator<<(std::ostream& os, const vec2& v);
std::ostream& operator<<(std::ostream& os, const vec3& v);
std::ostream& operator<<(std::ostream& os, const vec4& v);
std::ostream& operator<<(std::ostream& os, const mat2& m);
std::ostream& operator<<(std::ostream& os, const mat3& m);
std::ostream& operator<<(std::ostream& os, const mat4& m);
std::ostream& operator<<(std::ostream& os, const vec3_vector& v);

GLfloat max_abs_component(const vec3& v);

//--------------------------------------------------------------------
template <typename _It, typename _OutIt>
void generate_face_normals(_It beg, _It end, _OutIt out) {
  typedef decltype(*beg) value_type;
  unsigned size = end - beg;
  if (size % 3 != 0) {
    throw std::runtime_error("number of vertices should be multiple of 3");
  }
  for (_It iter = beg; iter != end;) {
    value_type& v0 = *iter++;
    value_type& v1 = *iter++;
    value_type& v2 = *iter++;
    vec3 normal = face_normal(v0, v1, v2);
    *out++ = normal;
    *out++ = normal;
    *out++ = normal;
  }
}

// p0 and p1 is in window space
glm::mat4 arcball(const glm::vec2& p0, const glm::vec2& p1,
  const glm::mat4& w_mat_i, GLfloat radius = 0.8f);

glm::vec3 ndc_tosphere(const glm::vec2& p, GLfloat radius = 0.8f);

glm::mat4 compute_window_mat(
  GLint x, GLint y, GLint width, GLint height, GLfloat n = -1, GLfloat f = 1);

glm::mat4 compute_window_mat_i(
  GLint x, GLint y, GLint width, GLint height, GLfloat n, GLfloat f);

void set_row(mat4& m, GLushort index, const vec4& v);
void set_row(mat4& m, GLushort index, const vec3& v);
void set_col(mat4& m, GLushort index, const vec3& v);
mat4 erase_translation(const glm::mat4& m);

mat4 make_mat4_row(
  const vec4& r0, const vec4& r1, const vec4& r2, const vec4& r3);
mat4 make_mat4_row(
  const vec3& r0, const vec3& r1, const vec3& r2);
mat3 make_mat3_row(
  const vec3& r0, const vec3& r1, const vec3& r2);

inline GLfloat pi(GLfloat a0, GLfloat d, uint n) {
  GLfloat m = 1;
  while (n--) m *= a0 + n * d;  // reverse order of pi
  return m;
}

// get camera position in world space, no need to invert view matrix.
vec3 eye_pos(const mat4& v_mat);
GLfloat line_point_distance2(const vec3& point, const vec3& line_point, const vec3& line_direction);
GLfloat line_point_distance(const vec3& point, const vec3& line_point, const vec3& line_direction);
// return -1 if point resides in negative direction
GLfloat ray_point_distance2(const vec3& point, const vec3& ray_start, const vec3& line_direction);
GLfloat ray_point_distance(const vec3& point, const vec3& ray_start, const vec3& line_direction);


}

#endif /* GLM_H */
