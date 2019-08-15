#include "cone.h"

#include "common.h"
#include <algorithm>

namespace zxd
{

//--------------------------------------------------------------------
void cone::build_vertex()
{
  auto step_angle = f2pi / m_slice;
  auto height_step = m_height / m_stack;
  auto radius_step = m_radius / m_stack;

  auto vertices = make_array<vec3_array>(0);
  vertices->reserve(m_slice + 2 + (m_stack + 1) * (m_slice + 1));

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

  // create elements for stacks
  auto element = make_element<uint_array>();
  element->reserve(m_stack * 2 * (m_slice + 2));
  for (auto i = 0u; i < m_stack; ++i)
  {
    auto stack0 = m_slice + 2 + (m_slice + 1) * i;
    auto stack1 = stack0 + m_slice + 1;
    for (auto j = 0u; j <= m_slice; ++j)
    {
      element->push_back(stack0 + j);
      element->push_back(stack1 + j);
    }
    element->push_back(-1);
  }

  m_primitive_sets.clear();
  add_primitive_set(new draw_arrays(GL_TRIANGLE_FAN, 0, m_slice+2));
  add_primitive_set(new draw_elements(
    GL_TRIANGLE_STRIP, m_stack * 2 * (m_slice + 2), GL_UNSIGNED_INT, 0));
}

//--------------------------------------------------------------------
void cone::build_normal()
{
  auto normals = make_array<vec3_array>(1);
  normals->reserve(num_vertices());
  auto vertices = attrib_vec3_array(0);

  GLuint num_vert_bottom = m_slice + 2;

  // normals for bottom
  for (int i = 0; i < num_vert_bottom; ++i)
  {
    normals->push_back(vec3(0, 0, -1));
  }

  glm::vec3 apex(0, 0, m_height);

  // normals for 1st stack
  for (int i = 0; i <= m_slice; ++i)
  {

    // 1st circle is the same as apex, so we use last here.
    const vec3& vertex = (*vertices)[vertices->size() - (m_slice + 1) + i];
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

  assert(normals->size() == num_vertices());
}

//--------------------------------------------------------------------
void cone::build_texcoord()
{
  auto texcoords = make_array<vec2_array>(num_arrays());
  texcoords->reserve(num_vertices());

  // bottom texcoords
  texcoords->push_back(glm::vec2(0, 0));
  for (int i = 0; i <= m_slice; ++i)
  {
    GLfloat s = 1 - static_cast<GLfloat>(i) / m_slice;
    texcoords->push_back(glm::vec2(s, 0));
  }

  for (int i = 0; i <= m_stack; ++i)
  {
    GLfloat t = static_cast<GLfloat>(i) / m_stack;

    for (int j = 0; j <= m_slice; j++)
    {
      // loop last stack in reverse order
      GLfloat s = static_cast<GLfloat>(j) / m_slice;
      texcoords->push_back(glm::vec2(s, t));
    }
  }

  assert(texcoords->size() == num_vertices());
}

//--------------------------------------------------------------------
void cone::on_draw()
{
  glEnable(GL_PRIMITIVE_RESTART_FIXED_INDEX);
}

}
