#ifndef GL_GLC_GLM_H
#define GL_GLC_GLM_H
#define GLM_FORCE_SWIZZLE
#define GLM_ENABLE_EXPERIMENTAL
#define GLM_META_PROG_HELPERS

#include <vector>
#include <list>
#include <iostream>
#include <utility>

#include "gl.h"
#include <glm/fwd.hpp>
#include <glm/glm.hpp>

#include <glm/glm.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtc/noise.hpp>

#include <glm/gtc/constants.hpp>
#include <glm/gtc/random.hpp>

namespace zxd
{
using namespace glm;

// no static, const variable in namespace has internal linkage
const GLfloat fpi = glm::pi<GLfloat>();
const GLfloat f2pi = fpi * 2;
const GLfloat fpi2 = fpi / 2;
const GLfloat fpi4 = fpi / 4;
const GLfloat fpi8 = fpi / 8;
const GLfloat fpi16 = fpi / 16;
const GLfloat fpi32 = fpi / 32;
const GLdouble dpi = glm::pi<GLdouble>();
const GLfloat d2pi = dpi * 2;
const GLfloat dpi2 = dpi / 2;
const GLfloat dpi4 = dpi / 4;
const GLfloat dpi8 = dpi / 8;
const GLfloat dpi16 = dpi / 16;
const GLfloat dpi32 = dpi / 32;

const glm::vec3 pxa(1, 0, 0);
const glm::vec3 nxa(-1, 0, 0);
const glm::vec3 pya(0, 1, 0);
const glm::vec3 nya(0, -1, 0);
const glm::vec3 pza(0, 0, 1);
const glm::vec3 nza(0, 0, -1);
const glm::vec4 hpxa(1, 0, 0, 1);
const glm::vec4 hnxa(-1, 0, 0, 1);
const glm::vec4 hpya(0, 1, 0, 1);
const glm::vec4 hnya(0, -1, 0, 1);
const glm::vec4 hpza(0, 0, 1, 1);
const glm::vec4 hnza(0, 0, -1, 1);
const glm::vec3 zp(0, 0, 0);
const glm::vec4 hzp(0, 0, 0, 1);

using mat2_vector = std::vector<mat2>;
using mat3_vector = std::vector<mat3>;
using mat4_vector = std::vector<mat4>;
using int_vector = std::vector<GLint>;
using uint_vector = std::vector<GLuint>;
using short_vector = std::vector<GLshort>;
using ushort_vector = std::vector<GLushort>;
using float_vector = std::vector<GLfloat>;
using vec2_vector = std::vector<vec2>;
using vec3_vector = std::vector<vec3>;
using vec4_vector = std::vector<vec4>;
using vec2_list = std::list<vec2>;
using vec3_list = std::list<vec3>;
using vec4_list = std::list<vec4>;
using uvec3_vector = std::vector<uvec3>;

using float_vector2 = std::vector<std::vector<GLfloat> >;
using vec2_vector2 = std::vector<std::vector<vec2> >;
using vec3_vector2 = std::vector<std::vector<vec3> >;
using vec4_vector2 = std::vector<std::vector<vec4> >;

using fv_i = float_vector::iterator;
using v2v_i = vec2_vector::iterator;
using v3v_i = vec3_vector::iterator;
using v4v_i = vec4_vector::iterator;
using fv_ci = float_vector::const_iterator;
using v2v_ci = vec2_vector::const_iterator;
using v3v_ci = vec3_vector::const_iterator;
using v4v_ci = vec4_vector::const_iterator;

// create a cube in 1st quadrant, stick it's 3 laterials to frame axes, look at
// origin from the longest vertex, the angle between any two projected axes
// should be 120
mat4 isometric_projection(GLfloat d, const vec3& center = vec3(0), const vec3& up = pza);

// follow convention of glm, always return
vec3 make_floor(const vec3& lhs, const vec3& rhs);

vec3 make_ceil(const vec3& lhs, const vec3& rhs);

vec3 axis(const mat4& m, GLuint i);

mat4 create_mat4(const vec3& xa, const vec3& ya, const vec3& za);

// radian between any vector, no need to normalize first
GLfloat angle_any(const vec3& lhs, const vec3& rhs);

GLfloat random();

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

mat4 rotate_to_any(const vec3& v0, const vec3& v1);

// rotate dir by random orthogonal axis in angle, assume dir normalized
vec3 rotate_in_cone(const vec3& ndir, GLfloat angle);

// dir must be normalized!
vec3 random_orthogonal(const vec3& ndir);

float gaussian_weight(double x, double mean, double deviation);

// use rect center as origin, hw, hh as minimum half width and height, adjusted
// according to aspect
mat4 rect_ortho(GLfloat hw, GLfloat hh, GLfloat aspect, GLfloat n = -1, GLfloat f = 1);

vec3 rgb2hsb(const vec3& c);

vec3 clamp_length(const vec3& v, GLfloat min_lenth,  GLfloat max_length);

vec2 rotate(GLfloat angle, const vec2& v);

//  Function from Iñigo Quiles
//  https://www.shadertoy.com/view/MsS3Wc
vec3 hsb2rgb(const vec3& c);

float rgb2luminance(const vec3& c, const vec3& weight = vec3(0.3086, 0.6094, 0.0820));

GLfloat color_difference2(const vec3& c0, const vec3& c1);
GLfloat color_difference_256_2(const vec3& c0, const vec3& c1);

// assume w is 1
bool operator<(const vec3& lhs, const vec3& rhs);

bool operator>(const vec3& lhs, const vec3& rhs);

GLfloat max_abs_component(const vec3& v);

//--------------------------------------------------------------------
template <typename _It, typename _OutIt>
void generate_face_normals(_It beg, _It end, _OutIt out)
{
  typedef decltype(*beg) value_type;
  unsigned size = end - beg;
  if (size % 3 != 0)
  {
    throw std::runtime_error("number of vertices should be multiple of 3");
  }
  for (_It iter = beg; iter != end;)
  {
    value_type& v0 = *iter++;
    value_type& v1 = *iter++;
    value_type& v2 = *iter++;
    vec3 normal = face_normal(v0, v1, v2);
    *out++ = normal;
    *out++ = normal;
    *out++ = normal;
  }
}

GLuint gcd(GLuint a, GLuint b);
GLuint lcm(GLuint a, GLuint b);
// least common multiple of 1/a and 1/b
GLuint ilcm(GLuint a, GLuint b);

// assume x [0, 1]
GLfloat cubic_in_out(GLfloat x);

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

GLfloat pi(GLfloat a0, GLfloat d, uint n);

GLfloat triangle_area(const vec3& p0, const vec3& p1, const vec3& p2);

// get camera position in world space, no need to invert view matrix.
vec3 eye_pos(const mat4& v_mat);
}

namespace std
{
using namespace glm;
ostream& operator<<(ostream& os, const vec2& v);
ostream& operator<<(ostream& os, const vec3& v);
ostream& operator<<(ostream& os, const vec4& v);
ostream& operator<<(ostream& os, const mat2& m);
ostream& operator<<(ostream& os, const mat3& m);
ostream& operator<<(ostream& os, const mat4& m);
ostream& operator<<(ostream& os, const zxd::vec3_vector& v);
}

#endif /* GL_GLC_GLM_H */
