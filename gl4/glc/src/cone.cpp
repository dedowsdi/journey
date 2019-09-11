#include <cone.h>

#include <algorithm>

#include <common.h>
#include <geometry_util.h>

namespace zxd
{

cone::cone(GLfloat radius, GLfloat height, GLuint slice, GLuint stack)
    : m_radius(radius), m_height(height), m_slice(slice), m_stack(stack)
{
}

//--------------------------------------------------------------------
common_geometry::vertex_build cone::build_vertices()
{
  auto step_angle = f2pi / m_slice;
  auto height_step = m_height / m_stack;
  auto radius_step = m_radius / m_stack;

  auto vertices = std::make_unique<vec3_array>();
  auto num_vertices = m_slice + 2 + (m_stack + 1) * (m_slice + 1);
  vertices->reserve(num_vertices);

  // create bottom as triangle fan in cw order
  vertices->push_back(vec3(0));
  for (auto i = 0u; i <= m_slice; ++i)
  {
    auto angle = i == m_slice ? 0 : -step_angle * i;
    vertices->push_back(vec3(cos(angle) * m_radius, sin(angle) * m_radius, 0));
  }

  // create stack from top to bottom
  for (auto i = 0u; i <= m_stack; ++i)
  {
    GLfloat h = m_height - height_step * i;
    GLfloat r = radius_step * i;

    std::transform(vertices->rend() - (m_slice + 2), vertices->rend() - 1,
      std::back_inserter(*vertices),
      [=](const auto &pos) -> vec3 { return vec3(pos.x * r, pos.y * r, h); });
  }

  assert(vertices->size() == num_vertices);

  // create elements for stacks
  auto& elements = make_element<uint_array>();

  auto num_elements = m_stack * 2 * (m_slice + 1) + m_stack;
  elements.reserve(num_elements);
  build_strip_elements(elements, m_stack, m_slice, m_slice + 2);
  assert(elements.size() == num_elements);

  clear_primitive_sets();
  add_primitive_set(std::make_shared<draw_arrays>(GL_TRIANGLE_FAN, 0, m_slice+2));
  add_primitive_set(std::make_shared<draw_elements>(
    GL_TRIANGLE_STRIP, m_stack * 2 * (m_slice + 2), GL_UNSIGNED_INT, 0));
  return vertex_build{std::move(vertices)};
}

//--------------------------------------------------------------------
array_uptr cone::build_normals(const array& vertices)
{
  auto normals = std::make_unique<vec3_array>();
  normals->reserve(vertices.size());

  GLuint num_vert_bottom = m_slice + 2;

  // normals for bottom
  for (int i = 0; i < num_vert_bottom; ++i)
  {
    normals->push_back(vec3(0, 0, -1));
  }

  glm::vec3 apex(0, 0, m_height);

  auto& v = static_cast<const vec3_array&>(vertices);
  // normals for 1st stack
  for (int i = 0; i <= m_slice; ++i)
  {

    // 1st circle is the same as apex, so we use last here.
    const vec3& vertex = v[v.size() - (m_slice + 1) + i];
    vec3 outer = glm::cross(vertex, apex);
    vec3 normal = glm::cross(apex - vertex, outer);
    normals->push_back(normal);
  }

  for (int i = 1; i <= m_stack; ++i)
  {
    // no reallocate will happen here
    normals->insert(normals->end(), normals->begin() + num_vert_bottom,
      normals->begin() + num_vert_bottom + (m_slice + 1));
  }

  assert(normals->size() == vertices.size());
  return normals;
}

//--------------------------------------------------------------------
array_uptr cone::build_texcoords(const array& vertices)
{
  auto texcoords = std::make_unique<vec2_array>();
  texcoords->reserve(vertices.size());

  // bottom texcoords
  texcoords->push_back(glm::vec2(0, 0));
  for (int i = 0; i <= m_slice; ++i)
  {
    GLfloat s = 1 - static_cast<GLfloat>(i) / m_slice;
    texcoords->push_back(glm::vec2(s, 0));
  }

  build_strip_texcoords(*texcoords, m_stack, m_slice, 1, 0);

  assert(texcoords->size() == vertices.size());
  return texcoords;
}

}
