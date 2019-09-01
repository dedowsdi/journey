#include "capsule3d.h"

#include <algorithm>
#include <functional>

#include <glm/trigonometric.hpp>
#include <glm/gtx/norm.hpp>
#include "sphere.h"
#include "geometry_util.h"

namespace zxd
{

//--------------------------------------------------------------------
void capsule3d::build_vertex()
{
  if(m_sphere_slice & 1)
    m_sphere_slice += 1;
  if(m_sphere_stack & 1)
    m_sphere_stack += 1;

                               // create vertices

  auto vertices = make_array<vec3_array>(0);
  auto num_vertices = (m_sphere_stack + 2) * (m_sphere_slice + 1) +
                      (m_cylinder_stack + 1) * (m_sphere_slice + 1);
  vertices->reserve(num_vertices);

  auto normals = std::make_shared<vec3_array>();
  normals->reserve(num_vertices);

  const vec3_vector sphere_points =
    sphere::create_points(m_radius, m_sphere_slice, m_sphere_stack);

  auto half_sphere_points = (m_sphere_stack / 2 + 1) * (m_sphere_slice + 1);
  vec3 origin = vec3(0, 0, m_height * 0.5f - m_radius);

  // top half sphere
  std::transform(sphere_points.begin(),
    sphere_points.begin() + half_sphere_points, std::back_inserter(*vertices),
    std::bind(std::plus<vec3>(), origin, std::placeholders::_1));

  // bottom half sphere
  std::transform(sphere_points.end() - half_sphere_points, sphere_points.end(),
    std::back_inserter(*vertices),
    std::bind(std::plus<vec3>(), -origin, std::placeholders::_1));

  // cylinder
  auto height_step = origin.z * 2.0f / m_cylinder_stack;
  for (auto i = 0u; i <= m_cylinder_stack; ++i) 
  {
    std::transform(
      sphere_points.begin() + (half_sphere_points - m_sphere_slice - 1),
      sphere_points.begin() + half_sphere_points, std::back_inserter(*vertices),
      std::bind(std::plus<vec3>(), origin - vec3(0, 0, height_step * i),
        std::placeholders::_1));
  }

  auto norm = [](const auto& v) -> vec3 { return normalize(v); };

  // top half sphere normals
  std::transform(sphere_points.begin(),
    sphere_points.begin() + half_sphere_points, std::back_inserter(*normals),
    norm);

  // bottom half sphere normals
  std::transform(sphere_points.end() - half_sphere_points, sphere_points.end(),
    std::back_inserter(*normals), norm);

  // cylinder normals
  for (auto i = 0u; i <= m_cylinder_stack; ++i)
  {
    std::transform(
      sphere_points.begin() + (half_sphere_points - m_sphere_slice - 1),
      sphere_points.begin() + half_sphere_points, std::back_inserter(*normals),
      norm);
  }

  if (include_normal())
  {
    attrib_array(num_arrays(), normals);
  }

  assert(vertices->size() == num_vertices);
  assert(normals->size() == num_vertices);

                              // create elemenets

  auto elements = make_element<uint_array>();
  auto num_elements =
    (m_sphere_stack + m_cylinder_stack) * 2 * (m_sphere_slice + 1) +
    m_sphere_stack + m_cylinder_stack;
  elements->reserve(num_elements);

  // top half sphere
  build_strip_elements(*elements, m_sphere_stack / 2, m_sphere_slice);

  // bottom half sphere
  build_strip_elements(
    *elements, m_sphere_stack / 2, m_sphere_slice, half_sphere_points);

  // cylinder
  build_strip_elements(
    *elements, m_cylinder_stack, m_sphere_slice, half_sphere_points * 2);

  assert(elements->size() == num_elements);

  m_primitive_sets.clear();
  add_primitive_set(
    new draw_elements(GL_TRIANGLE_STRIP, elements->size(), GL_UNSIGNED_INT, 0));
}

//--------------------------------------------------------------------
void capsule3d::build_texcoord()
{
  auto texcoords = make_array<vec2_array>(num_arrays());
  texcoords->reserve(num_vertices());

  GLfloat bot_origin_t = m_radius / m_height;
  GLfloat top_origin_t = 1 - bot_origin_t;

  // top half sphere
  build_strip_texcoords(
    *texcoords, m_sphere_stack/2, m_sphere_slice, 1.0, top_origin_t);

  // bottom half sphere
  build_strip_texcoords(
    *texcoords, m_sphere_stack/2, m_sphere_slice, bot_origin_t, 0);

  // texcoords for cylinder
  build_strip_texcoords(
    *texcoords, m_cylinder_stack, m_sphere_slice, top_origin_t, bot_origin_t);

  assert(texcoords->size() == num_vertices());
}

}
