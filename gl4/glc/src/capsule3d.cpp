#include <capsule3d.h>

#include <algorithm>
#include <functional>

#include <glm/trigonometric.hpp>
#include <glm/gtx/norm.hpp>
#include <sphere.h>
#include <geometry_util.h>

namespace zxd
{

//--------------------------------------------------------------------
common_geometry::vertex_build capsule3d::build_vertices()
{
  if(m_sphere_slice & 1)
    m_sphere_slice += 1;
  if(m_sphere_stack & 1)
    m_sphere_stack += 1;

                               // create vertices

  auto vertices = std::make_unique<vec3_array>();
  auto num_vertices = (m_sphere_stack + 2) * (m_sphere_slice + 1) +
                      (m_cylinder_stack + 1) * (m_sphere_slice + 1);
  vertices->reserve(num_vertices);

  auto normals = std::make_unique<vec3_array>();
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

  if (has_normal())
  {
    assert(vertices->size() == num_vertices);
    assert(normals->size() == num_vertices);
  }

  // create elemenets

  auto& elements = make_element<uint_array>();
  auto num_elements =
    (m_sphere_stack + m_cylinder_stack) * 2 * (m_sphere_slice + 1) +
    m_sphere_stack + m_cylinder_stack;
  elements.reserve(num_elements);

  // top half sphere
  build_strip_elements(elements, m_sphere_stack / 2, m_sphere_slice);

  // bottom half sphere
  build_strip_elements(
    elements, m_sphere_stack / 2, m_sphere_slice, half_sphere_points);

  // cylinder
  build_strip_elements(
    elements, m_cylinder_stack, m_sphere_slice, half_sphere_points * 2);

  assert(elements.size() == num_elements);

  clear_primitive_sets();
  add_primitive_set(std::make_shared<draw_elements>(
    GL_TRIANGLE_STRIP, elements.size(), GL_UNSIGNED_INT, 0));

  std::map<attrib_semantic, array_uptr> m;
  m.insert(std::make_pair(attrib_semantic::vertex, std::move(vertices)));
  if (has_normal())
    m.insert(std::make_pair(attrib_semantic::normal, std::move(normals)));
  return std::move(m);
}

//--------------------------------------------------------------------
array_uptr capsule3d::build_texcoords(const array& vertices)
{
  auto texcoords = std::make_unique<vec2_array>();
  texcoords->reserve(vertices.size());

  GLfloat bot_origin_t = m_radius / m_height;
  GLfloat top_origin_t = 1 - bot_origin_t;

  // top half sphere
  build_strip_texcoords(
    *texcoords, m_sphere_stack/2, m_sphere_slice, vec2(0, 1.0), vec2(1, top_origin_t));

  // bottom half sphere
  build_strip_texcoords(
    *texcoords, m_sphere_stack/2, m_sphere_slice, vec2(0, bot_origin_t), vec2(1, 0));

  // texcoords for cylinder
  build_strip_texcoords(
    *texcoords, m_cylinder_stack, m_sphere_slice, vec2(0, top_origin_t), vec2(1, bot_origin_t));

  assert(texcoords->size() == vertices.size());
  return texcoords;
}

}
