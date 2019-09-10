#include "torus.h"
#include "geometry_util.h"
#include "common.h"

namespace zxd
{

//--------------------------------------------------------------------
torus::torus()
    : m_inner_radius(0.25), m_outer_radius(1), m_sides(16), m_rings(16)
{
}

//--------------------------------------------------------------------
torus::torus(
  GLfloat inner_radius, GLfloat outer_radius, GLuint sides, GLuint rings)
    : m_inner_radius(inner_radius), m_outer_radius(outer_radius),
      m_sides(sides), m_rings(rings)
{
}

//--------------------------------------------------------------------
common_geometry::vertex_build torus::build_vertices()
{
                               // build vertices

  auto vertices = std::make_unique<vec3_array>();
  auto num_vertices = (m_rings + 1) * (m_sides + 1);
  vertices->reserve(num_vertices);

  auto normals = std::make_unique<vec3_array>();
  auto texcoords = std::make_unique<vec2_array>();
  normals->reserve(num_vertices);
  texcoords->reserve(num_vertices);

  GLfloat theta_step = f2pi / m_rings;
  GLfloat phi_step = f2pi / m_sides;

  for (int i = 0; i <= m_rings; ++i)
  {
    auto theta = i == m_rings ? 0 : theta_step * i;
    auto cos_theta = cos(theta);
    auto sin_theta = sin(theta);
    vec3 ring_center = vec3(cos_theta, sin_theta, 0) * m_outer_radius;
    auto s = static_cast<GLfloat>(i) / m_rings;

    for (int j = 0; j <= m_sides; ++j)
    {
      GLfloat phi = phi_step * j;
      GLfloat r = m_outer_radius + m_inner_radius * cos(phi);
      vertices->push_back(vec3(cos_theta * r, sin_theta * r, m_inner_radius * sin(phi)));
      normals->push_back(normalize(vertices->back() - ring_center));
      auto t = static_cast<GLfloat>(j) / m_sides;
      texcoords->push_back(vec2(s, t));
    }
  }

  assert(vertices->size() == num_vertices);
  assert(normals->size() == num_vertices);
  assert(texcoords->size() == num_vertices);

  // build elements
  auto& elements = make_element<uint_array>();
  auto num_elements = m_rings * 2 * (m_sides + 1) + m_rings;
  elements.reserve(num_elements);
  build_strip_elements(elements, m_rings, m_sides);
  assert(elements.size() == num_elements);

  clear_primitive_sets();
  add_primitive_set(std::make_shared<draw_elements>(
    GL_TRIANGLE_STRIP, elements.size(), GL_UNSIGNED_INT, 0));

  std::map<attrib_semantic, array_uptr> m;
  m.insert(std::make_pair(attrib_semantic::vertex, std::move(vertices)));
  if (has_normal())
    m.insert(std::make_pair(attrib_semantic::normal, std::move(normals)));
  if (has_texcoord())
    m.insert(std::make_pair(attrib_semantic::texcoord, std::move(texcoords)));
  return std::move(m);
}
}
