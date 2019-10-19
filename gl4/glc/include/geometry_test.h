#ifndef GL4_GLC_GEOMETRY_TEST_H
#define GL4_GLC_GEOMETRY_TEST_H

#include <utility>

#include <gl.h>
#include <glm.h>

namespace geometry_test
{
using namespace glm;

using bool_vec2_pair = std::pair<bool, vec2>;
using bool_vec3_pair = std::pair<bool, vec3>;
using int_vec3_pair = std::pair<int, vec3>;
using plane = vec4; // x,y,z as normal, w as d. dot(n, p) + d = 0

const GLfloat fepsilon = 0.00001f;
const GLfloat fnepsilon = -0.00001f;

inline bool is_aboutf( GLfloat t0, GLfloat t1)
{
  GLfloat f = t0 - t1;
  return f < fepsilon && f > fnepsilon;
}

bool is_aboutf(const vec3& t0, const vec3& t1) ;

inline bool is_aboutd( GLdouble t0, GLdouble t1, double epsilon = 0.000001)
{
  GLfloat d = t0 - t1;
  return d < fepsilon && d > fnepsilon;
}

// all assume ld normalized
vec3 point_line_projection(const vec3& p, const vec3& lp, const vec3& ld);
GLfloat point_line_distance2(const vec3& p, const vec3& lp, const vec3& ld);
GLfloat point_line_distance(const vec3& p, const vec3& lp, const vec3& ld);
vec3 point_line_reflect(const vec3& p, const vec3& lp, const vec3& ld);

// all assume plane normal normalized
vec3 point_plane_projection(const vec3& po, const vec4& pl);
vec3 point_plane_reflect(const vec3& po, const vec4& pl);

vec3 point_point_reflect(const vec3& reference_point, const vec3& reflect_point);

struct pli_res
{
  bool intersect;
  GLfloat coefficient;
  vec3 point;
};
pli_res plane_line_intersect(const vec3& lp, const vec3& ld, const vec4& pl, GLfloat epsilon = 0.000002f);

// return -1 if point resides in negative direction
GLfloat point_ray_distance2(const vec3& p, const vec3& ray_start, const vec3& ld);
GLfloat point_ray_distance(const vec3& p, const vec3& ray_start, const vec3& ld);

GLfloat line_line_distance(const vec3& p0, const vec3& d0, const vec3& p1, const vec3& d1);

bool is_point_in_triangle(const vec3& p, const vec3& c0, const vec3& c1, const vec3& c2);

// ld and pn need not be normalized
// result.first is true if there are one and only one intersection
bool_vec3_pair line_plane_intersect(const vec3& lp, const vec3& ld, const vec4& plane);

struct line_relation
{
  GLint type;  // 0 parallel, 1 intersect, 2 skew
  GLfloat t0;  // v0 + dir0 * t0 = sk0
  GLfloat t1;  // v1 + dir1 * t1 = sk1
  vec3 sk0;
  vec3 sk1;
};

line_relation line_line_intersect(const vec3& p0, const vec3& d0, const vec3& p1, const vec3& d1);

bool parallel(const vec3& v0, const vec3& v1);

}


#endif /* GL4_GLC_GEOMETRY_TEST_H */
