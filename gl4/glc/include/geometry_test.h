#ifndef GL4_GLC_GEOMETRY_TEST_H
#define GL4_GLC_GEOMETRY_TEST_H

#include <utility>

#include "gl.h"
#include "glm.h"

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

GLfloat line_point_distance2(const vec3& point, const vec3& line_point, const vec3& line_direction);
GLfloat line_point_distance(const vec3& point, const vec3& line_point, const vec3& line_direction);
// return -1 if point resides in negative direction
GLfloat ray_point_distance2(const vec3& point, const vec3& ray_start, const vec3& line_direction);
GLfloat ray_point_distance(const vec3& point, const vec3& ray_start, const vec3& line_direction);

GLfloat line_line_distance(const vec3& p0, const vec3& d0, const vec3& p1, const vec3& d1);

bool is_point_in_triangle(const vec3& p, const vec3& c0, const vec3& c1, const vec3& c2);

// ld and pn need not be normalized
// result.first is true if there are one and only one intersection
bool_vec3_pair intersect_line_plane(const vec3& lp, const vec3& ld, const vec4& plane);

struct line_relation
{
  GLint type;  // 0 parallel, 1 intersect, 2 skew
  GLfloat t0;  // v0 + dir0 * t0 = sk0
  GLfloat t1;  // v1 + dir1 * t1 = sk1
  vec3 sk0;
  vec3 sk1;
};

line_relation intersect_line_line(const vec3& p0, const vec3& d0, const vec3& p1, const vec3& d1);

bool parallel(const vec3& v0, const vec3& v1);

}


#endif /* GL4_GLC_GEOMETRY_TEST_H */
