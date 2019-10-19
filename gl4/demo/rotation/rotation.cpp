#include <iostream>
#include <iomanip>
#include <fstream>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/norm.hpp>
#include <timer.h>
#include <stream_util.h>
#include <geometry_test.h>
#include <glmath.h>

const auto rounds = 1<<16;

bool similar( GLfloat lhs, GLfloat rhs, GLfloat similar_epsison = 0.0001f)
{
  return abs(lhs - rhs) < similar_epsison;
}

template <glm::length_t L, typename T, glm::qualifier Q>
bool similar(const glm::vec<L, T, Q>& lhs, const glm::vec<L, T, Q>& rhs,
  GLfloat similar_epsison = 0.0001f)
{
  for (auto i = 0; i < L; ++i)
  {
    if (glm::abs(lhs[i] - rhs[i]) > similar_epsison)
    {
      return false;
    }
  }

  return true;
}

template <glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
bool similar(const glm::mat<C, R, T, Q>& lhs, const glm::mat<C, R, T, Q>& rhs,
  GLfloat similar_epsison = 0.0001f)
{
  for (auto i = 0; i < C; ++i)
  {
    for (auto j = 0; j < R; ++j)
    {
      if (glm::abs(lhs[i][j] - rhs[i][j]) > similar_epsison)
      {
        return false;
      }
    }
  }

  return true;
}

bool is_point_on_plane(const glm::vec3& point, const glm::vec4& plane, GLfloat epsilon = 0.0001f)
{
  auto n = glm::vec3(plane);
  return similar(glm::dot(n, point) + plane.w, 0, epsilon);
}

bool is_point_on_line(const glm::vec3& p, const glm::vec3& lp,
  const glm::vec3& ld, GLfloat epsilon = 0.0001f)
{
  auto v = p - lp;
  if (glm::length2(v) < 0.00001f)
    return true;

  return similar(abs(glm::dot(glm::normalize(v), ld)), 1.0f, epsilon);
}


template <glm::length_t L, typename T, glm::qualifier Q>
bool similar_direction(const glm::vec<L, T, Q>& lhs, const glm::vec<L, T, Q>& rhs,
  GLfloat similar_epsison = 0.0001f)
{
  auto n0 = glm::normalize(lhs);
  auto n1 = glm::normalize(rhs);
  return dot(n0, n1) >= 1 - similar_epsison;
}

void report(
  const std::string& test_name, double ms, GLint true_count, GLint false_count)
{
  std::cout << "took " << ms << " milliseconds to compute "
            << rounds << " rounds " << test_name << " : \n"
            << "    true count : " << true_count << "\n"
            << "    false count : " << false_count << std::endl;
}

// rotate x axis to proj of v on xy plane. This is the simplest case, in realiy you
// might need to rotate
//      -x  :   auto c = -proj.x; // add pi based on x
//              auto s = -proj.y;
//          
//       y  :   auto c = proj.y; // minus -0.5pi based on x
//              auto s = -proj.x;
//
//      -y  :   auto c = -proj.y; // plus 0.5pi based on x
//              auto s = proj.x;
// 
// Similar rules apply if you roate along other main axis.
//
glm::mat4 rotate_to_along_zaxis(const glm::vec3& v)
{
  auto proj = glm::normalize(glm::vec2(v));
  auto c = proj.x;
  auto s = proj.y;
  glm::mat4 m(1.0f);
  m[0][0] = c;
  m[0][1] = s;

  m[1][0] = -s;
  m[1][1] = c;
  return m;
}

void test_rotate_to_along_zaxis(std::ostream& err_os)
{
  timer t;
  auto true_count = 0;
  auto false_count = 0;
  for (unsigned i = 0; i < rounds; ++i)
  {
    auto v0 = glm::sphericalRand(10.0f);
    auto m0 = rotate_to_along_zaxis(v0);
    auto v1 = glm::vec3(m0 * glm::vec4(1, 0, 0, 1));
    if (similar_direction(glm::vec2(v0), glm::vec2(v1)))
      ++true_count;
    else
    {
      ++false_count;
      using namespace zxd;
      err_os << "----------------------------------------------\n"
             << "test rotate_to at failed at round : " << i << "\n"
             << "v0 : " << v0 << "\n"
             << "v1 : " << v1 << "\n"
             << "rotate to result : \n" << m0 << "\n";
    }
  }
  report("rotate_to_along_zaxis", t.milliseconds(), true_count, false_count);
}

glm::mat4 rotate_to(
  const glm::vec3& v0, const glm::vec3& v1, GLfloat dot_epsilon = 0.000001f)
{
  auto nv0 = glm::normalize(v0);
  auto nv1 = glm::normalize(v1);
  auto c = dot(nv0, nv1);

  if(c >= (1 - dot_epsilon))
  {
    return glm::mat4(1);
  }
  else if(c <= -1 + dot_epsilon)
  {
    // find random axis that's perpendicular to nv0
    auto random_vec = glm::sphericalRand(1.0f);
    while (abs(dot(random_vec, nv0)) >= 0.99)
    {
      random_vec = glm::sphericalRand(1.0f);
    }
    auto axis = glm::cross(random_vec, nv0);
    return glm::rotate(glm::pi<GLfloat>(), axis);
  }
  return glm::rotate(glm::acos(c), glm::cross(nv0, nv1));
}

void test_rotate_to(std::ostream& err_os)
{
  timer t;
  auto true_count = 0;
  auto false_count = 0;
  auto epsilon = 0.01f; // precision sucks when nv0 = -nv1
  for (unsigned i = 0; i < rounds; ++i)
  {
    auto v0 = glm::sphericalRand(10.0f);
    auto v1 = glm::sphericalRand(10.0f);
    auto m0 = rotate_to(v0, v1);
    auto v2 = glm::vec3(m0 * glm::vec4(v0, 1));
    if (similar(v1, v2, epsilon))
      ++true_count;
    else
    {
      ++false_count;
      using namespace zxd;
      err_os << "----------------------------------------------\n"
             << "test rotate_to at failed at round : " << i << "\n"
             << "v0 : " << v0 << "\n"
             << "v1 : " << v1 << "\n"
             << "v2 : " << v2 << "\n"
             << "rotate to result : \n" << m0 << "\n";
    }
  }
  report("rotate_to", t.milliseconds(), true_count, false_count);
}


// up requirement:
//   dot(side, up) = 0
//   dot(side^forward, up) > 0
// If you need to manipute axis other then z or -z, you can post multiply a
// rotation to rotate axis to z
glm::mat4 lookat(const glm::vec3& eye, const glm::vec3& center, const glm::vec3& up)
{
  auto backward = glm::normalize(eye - center); // ev
  auto side = glm::normalize(glm::cross(up, backward));
  auto real_up = glm::normalize(glm::cross(backward, side));
  // camera transform is :
  //    glm::translate(eye) * glm::mat4(glm::mat3(side, real_up, backward))

  // be careful here, don't usec center-eye, it's world origin, not world
  // center. You may skip this if you don't need translation.
  auto world_origin =
    glm::vec3(dot(-eye, side), dot(-eye, real_up), dot(-eye, backward));

  return glm::mat4(
      side.x,         real_up.x,      backward.x,     0,
      side.y,         real_up.y,      backward.y,     0,
      side.z,         real_up.z,      backward.z,     0,
      world_origin.x, world_origin.y, world_origin.z, 1
      );
}

void test_lookat(std::ostream& err_os)
{
  timer t;
  auto true_count = 0;
  auto false_count = 0;
  for (unsigned i = 0; i < rounds; ++i)
  {
    auto center = glm::linearRand(glm::vec3(-10), glm::vec3(10));
    auto eye = glm::sphericalRand(30.0f);
    auto up = glm::sphericalRand(1.0f);
    while(glm::abs(glm::dot(up, (eye-center))) < 0.1f)
      up = glm::sphericalRand(1.0f);

    auto m0 = lookat(eye, center, up);
    auto m1 = glm::lookAt(eye, center, up);
    if (similar(m0, m1))
      ++true_count;
    else
    {
      ++false_count;
      using namespace zxd;
      err_os << "----------------------------------------------\n"
             << "test look at failed at round : " << i << "\n"
             << "center : " << center << "\n"
             << "eye : " << eye << "\n"
             << "up : " << up << "\n"
             << "lookat result : \n" << m0 << "\n"
             << "glm::lookat result : \n" << m1 << "\n";
    }
  }
  report("lookat", t.milliseconds(), true_count, false_count);
}

void test_point_line_projection(std::ostream& err_os)
{
  timer t;
  auto true_count = 0;
  auto false_count = 0;
  for (unsigned i = 0; i < rounds; ++i)
  {
    auto lp = glm::linearRand(glm::vec3(-10), glm::vec3(10));
    auto ld = glm::sphericalRand(1.0f);
    auto p = glm::linearRand(glm::vec3(-10), glm::vec3(10));
    while (glm::length2(p - lp) < 0.00001)
    {
      p = glm::linearRand(glm::vec3(-10), glm::vec3(10));
    }
    auto proj = geometry_test::point_line_projection(p, lp, ld);

    if (is_point_on_line(proj, lp, ld))
      ++true_count;
    else
    {
      ++false_count;
      using namespace zxd;
      err_os << "----------------------------------------------\n"
             << "test line point projection failed at round : " << i << "\n"
             << "lp : " << lp << "\n"
             << "ld : " << ld << "\n"
             << "p : " << p << "\n"
             << "proj : " << proj << "\n";
    }
  }
  report("point line projection", t.milliseconds(), true_count, false_count);
}

void test_point_plane_projection(std::ostream& err_os)
{
  timer t;
  auto true_count = 0;
  auto false_count = 0;
  std::string name = "point plane projection";
  for (unsigned i = 0; i < rounds; ++i)
  {
    glm::vec4 plane(glm::sphericalRand(1.0f), glm::linearRand(-10.0f, 10.0f));
    auto p = glm::linearRand(glm::vec3(-10), glm::vec3(10.0));
    auto proj = geometry_test::point_plane_projection(p, plane);

    if (is_point_on_plane(proj, plane))
      ++true_count;
    else
    {
      ++false_count;
      using namespace zxd;
      err_os << "----------------------------------------------\n"
             << name << "failed at round : " << i << "\n"
             << "plane : " << plane << "\n"
             << "point : " << p << "\n"
             << "proj : " << proj << "\n"
             ;
    }
  }
  report(name, t.milliseconds(), true_count, false_count);
}

void test_line_plane_intersect(std::ostream& err_os)
{
  timer t;
  auto true_count = 0;
  auto false_count = 0;
  std::string name = "line plane intersect";
  for (unsigned i = 0; i < rounds; ++i)
  {
    glm::vec4 plane(glm::sphericalRand(1.0f), glm::linearRand(-10.0f, 10.0f));
    auto ld = glm::sphericalRand(1.0f);
    while(dot(ld, glm::vec3(plane)) < 0.001f)
    {
      ld = glm::sphericalRand(1.0f);
    }
    auto lp = glm::linearRand( glm::vec3(-10), glm::vec3(10) );
    auto p = geometry_test::line_plane_intersect(lp, ld, plane);

    if (p.first && is_point_on_line(p.second, lp, ld, 0.01f) &&
        is_point_on_plane(p.second, plane, 0.01f))
      ++true_count;
    else
    {
      ++false_count;
      using namespace zxd;
      err_os << "----------------------------------------------\n"
             << name << " failed at round : " << i << "\n"
             << "plane : " << plane << "\n"
             << "lp : " << lp << "\n"
             << "ld : " << ld << "\n"
             << "intersection : " << p.second << "\n"
             ;
    }
  }
  report(name, t.milliseconds(), true_count, false_count);
}

void test_scale_along(std::ostream& err_os)
{
  timer t;
  auto true_count = 0;
  auto false_count = 0;
  std::string name = "scale along";
  for (unsigned i = 0; i < rounds; ++i)
  {
    auto n = glm::sphericalRand(1.0f);
    auto f = glm::linearRand(-10.0f, 10.0f);
    while(abs(f) < 0.0001f)
    {
      f = glm::linearRand(-10.0f, 10.0f);
    }
    auto m = zxd::scale_along(n, f);

    auto p = glm::linearRand(glm::vec3(-10.0f), glm::vec3(10.0f));
    auto scaled_p = glm::vec3(m * glm::vec4(p, 1));
    auto diff = normalize(scaled_p - p);

    if (similar(abs(glm::dot(diff, n)), 1.0f))
      ++true_count;
    else
    {
      ++false_count;
      using namespace zxd;
      err_os << "----------------------------------------------\n"
             << name << " failed at round : " << i << "\n"
             << "n : " << n << "\n"
             << "f : " << f << "\n"
             << "p : " << p << "\n"
             << "scaled_p : " << scaled_p << "\n"
             << "m : " << m << "\n"
             ;
    }
  }
  report(name, t.milliseconds(), true_count, false_count);
}

void test_project(std::ostream& err_os)
{
  timer t;
  auto true_count = 0;
  auto false_count = 0;
  std::string name = "project";
  for (unsigned i = 0; i < rounds; ++i)
  {
    glm::vec4 plane(glm::sphericalRand(1.0f), glm::linearRand(-10.0f, 10.0f));
    auto m = zxd::project(plane);

    auto p = glm::linearRand(glm::vec3(-10.0f), glm::vec3(10.0f));
    auto proj = glm::vec3(m * glm::vec4(p, 1));

    if (is_point_on_plane(proj, plane))
      ++true_count;
    else
    {
      ++false_count;
      using namespace zxd;
      err_os << "----------------------------------------------\n"
             << name << " failed at round : " << i << "\n"
             << "plane : " << plane << "\n"
             << "p : " << p << "\n"
             << "proj : " << proj << "\n"
             << "m : " << m << "\n"
             ;
    }
  }
  report(name, t.milliseconds(), true_count, false_count);
}

void test_reflect(std::ostream& err_os)
{
  timer t;
  auto true_count = 0;
  auto false_count = 0;
  std::string name = "reflect";
  for (unsigned i = 0; i < rounds; ++i)
  {
    glm::vec4 plane(glm::sphericalRand(1.0f), glm::linearRand(-10.0f, 10.0f));
    auto m = zxd::reflect(plane);

    auto p = glm::linearRand(glm::vec3(-10.0f), glm::vec3(10.0f));
    auto refl = glm::vec3(m * glm::vec4(p, 1));
    auto d = refl - p;

    if (similar(p, refl) ||
        similar(abs(dot(glm::normalize(d), glm::vec3(plane))), 1.0f, 0.1f))
      ++true_count;
    else
    {
      ++false_count;
      using namespace zxd;
      err_os << "----------------------------------------------\n"
             << name << " failed at round : " << i << "\n"
             << "plane : " << plane << "\n"
             << "p : " << p << "\n"
             << "refl : " << refl << "\n"
             << "m : " << m << "\n"
             ;
    }
  }
  report(name, t.milliseconds(), true_count, false_count);
}

int main(int argc, char *argv[])
{
  auto seed = time(0);
  srand(seed);
  std::ofstream error_os("rotation_error");
  error_os << "seed : " << seed << "\n";

  test_lookat(error_os);
  test_rotate_to(error_os);
  test_rotate_to_along_zaxis(error_os);
  test_point_line_projection(error_os);
  test_point_plane_projection(error_os);
  test_line_plane_intersect(error_os);
  test_scale_along(error_os);
  test_project(error_os);
  test_reflect(error_os);


  return 0;
}
