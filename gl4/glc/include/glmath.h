#ifndef GL_GLC_GLMATH_H
#define GL_GLC_GLMATH_H

#include "glm.h"

namespace zxd
{

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

vec3 clamp_length(const vec3& v, GLfloat min_lenth,  GLfloat max_length);

vec2 rotate(GLfloat angle, const vec2& v);

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

glm::mat4 compute_window_mat(GLint x, GLint y, GLint width, GLint height,
                             GLfloat depth_near = 0, GLfloat depth_far = 1);

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

#endif /* GL_GLC_GLMATH_H */
