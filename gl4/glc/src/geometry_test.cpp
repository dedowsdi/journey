#include <geometry_test.h>
#include <glm.h>
#include <glm/gtx/norm.hpp>

namespace geometry_test
{

//--------------------------------------------------------------------
bool is_aboutf(const vec3& t0, const vec3& t1) 
{
  vec3 v = abs(t0 - t1);
  return v.x < fepsilon && v.y < fepsilon && v.z < fepsilon;
}
//--------------------------------------------------------------------
GLfloat line_point_distance2(const vec3& point, const vec3& line_point, const vec3& line_direction)
{
  vec3 v = point - line_point;
  return length2(v - line_direction * glm::dot(v, line_direction));
}

//--------------------------------------------------------------------
GLfloat line_point_distance(const vec3& point, const vec3& line_point, const vec3& line_direction)
{
  return sqrt(line_point_distance2(point, line_point, line_direction));
}

//--------------------------------------------------------------------
GLfloat ray_point_distance2(const vec3& point, const vec3& ray_start, const vec3& line_direction)
{
  if(dot(line_direction, point - ray_start) < 0)
    return -1;

  return  line_point_distance2(point, ray_start, line_direction);
}

//--------------------------------------------------------------------
GLfloat ray_point_distance(const vec3& point, const vec3& ray_start, const vec3& line_direction)
{
  GLfloat d2 = ray_point_distance2(point, ray_start, line_direction);
  return d2 == -1 ? -1 : glm::sqrt(d2);
}

//--------------------------------------------------------------------
GLfloat line_line_distance(const vec3& p0, const vec3& d0, const vec3& p1, const vec3& d1)
{
  static GLfloat epsilon = 0.00001f;
  vec3 d2 = cross(d0, d1);

  // check parallel
  if (length2(d2) < epsilon)
    return line_point_distance(p0, d0, p1);

  // any line segment between two lines proj on d2 will be our distance
  d2 = normalize(d2);
  return abs(dot((p1 - p0), d2));
}

//--------------------------------------------------------------------
bool is_point_in_triangle(const vec3& p, const vec3& c0, const vec3& c1, const vec3& c2)
{
  // the area direction of internal triangle must be the same as the bigger one
  vec3 v01 = c1 - c0;
  vec3 v02 = c2 - c0;
  vec3 n = cross(v01, v02);
  if(dot(n, cross(v01, p - c0)) < 0)
    return false;
  if(dot(n, cross(c2 - c1, p - c1)) < 0)
    return false;
  if(dot(n, cross(-v02, p - c2)) < 0)
    return false;
  return true;
}

//--------------------------------------------------------------------
bool_vec3_pair intersect_line_plane(const vec3& lp, const vec3& ld, const vec4& plane)
{
  vec3 pn = vec3(plane);

  GLfloat dot_pn_ld = dot(pn, ld);
  if (abs(dot_pn_ld) < 0.00001f) // parallel check
    return std::make_pair(false, vec3());

  // solve (lp + t*ld)·pn + pd = 0
  GLfloat pd = plane.w;

  GLfloat t = (-pd - dot(lp, pn)) / dot_pn_ld;
  return std::make_pair(true, lp + ld * t);
}

//--------------------------------------------------------------------
line_relation intersect_line_line(const vec3& p0, const vec3& d0, const vec3& p1, const vec3& d1)
{
  vec3 v01 = p1 - p0;
  vec3 c01 = cross(d0, d1);

  line_relation res;

  // check parallel
  if (is_aboutf(c01, vec3(0)))
  {
    res.type = 0;
    return res;
  }

  float rpl2 = 1 / glm::length2(c01);

  res.t0 = (dot(cross(v01, d1), c01)) * rpl2;
  res.t1 = (dot(cross(v01, d0), c01)) * rpl2;

  res.sk0 = p0 + d0 * res.t0;
  res.sk1 = p1 + d1 * res.t1;

  // check skew or intersectio
  res.type = length2(res.sk0 - res.sk1) > fepsilon ? 2 : 1;

  return res;
}

//--------------------------------------------------------------------
bool parallel(const vec3& v0, const vec3& v1)
{
  vec3 v2 = cross(v0, v1);
  return v2.x < fepsilon && v2.y < fepsilon && v2.z < fepsilon &&
  v2.x > fnepsilon && v2.y > fnepsilon && v2.z > fnepsilon;
}

}
