#include "cylinder.h"

#include <algorithm>

#include "common.h"

namespace zxd
{

cylinder::cylinder()
    : m_base(1), m_top(1), m_height(1), m_slice(16), m_stack(16)
{
}

cylinder::cylinder(
  GLfloat base, GLfloat top, GLfloat height, GLuint slice, GLuint stack)
    : m_base(base), m_top(top), m_height(height), m_slice(slice), m_stack(stack)
{
}

//--------------------------------------------------------------------
void cylinder::build_vertex()
{
                               // create vertices

  auto vertices = make_array<vec3_array>(0);

  vertices->reserve(2 * (m_slice + 2) + m_stack * 2 * (m_slice + 1));

  GLfloat step_angle = f2pi / m_slice;

  // add top fan
  vertices->push_back(vec3(0, 0, m_height * 0.5f));
  for (auto i = 0u; i <= m_slice; ++i)
  {
    GLfloat phi = i == m_slice ? 0 : step_angle * i;
    vertices->push_back(
      vec3(m_top * cos(phi), m_top * sin(phi), m_height * 0.5f));
  }

  // add bottom fan
  vertices->push_back(vec3(0, 0, -m_height * 0.5f));
  for (auto i = 0; i <= m_slice; ++i)
  {
    GLfloat phi = i == m_slice ? 0 : step_angle * -i;
    vertices->push_back(
      vec3(m_base * cos(phi), m_base * sin(phi), -m_height * 0.5f));
  }

  // create stack from top to bottom
  auto height_step = m_height / m_stack;
  auto radius_step = (m_base - m_top) / m_stack;

  for (auto i = 0u; i <= m_stack; ++i)
  {
    GLfloat r = m_top + radius_step * i;
    GLfloat h = m_height * 0.5f - height_step *  i;
    for (auto j = 0u; j <= m_slice; j++)
    {
      GLfloat phi = j == 0 ? 0 : step_angle * j;
      vertices->push_back(vec3(r * cos(phi), r * sin(phi), h));
    }
  }

                         // create elements for stacks

  auto elements = make_element<uint_array>();
  for (auto i = 0u; i < m_stack; ++i)
  {
    auto stack0 = 2 * (m_slice + 2) + (m_slice + 1) * i;
    auto stack1 = stack0 + m_slice + 1;
    for (auto j = 0u; j <= m_slice; ++j)
    {
      elements->push_back(stack0 + j);
      elements->push_back(stack1 + j);
    }
    elements->push_back(-1);
  }

  m_primitive_sets.clear();

  add_primitive_set(new draw_arrays(GL_TRIANGLE_FAN, 0, m_slice + 2));
  add_primitive_set(new draw_arrays(GL_TRIANGLE_FAN, m_slice + 2, m_slice + 2));
  add_primitive_set(
    new draw_elements(GL_TRIANGLE_STRIP, elements->size(), GL_UNSIGNED_INT, 0));
}

//--------------------------------------------------------------------
void cylinder::build_normal()
{
  auto normals = make_array<vec3_array>(num_arrays());
  auto vertices = attrib_vec3_array(0);
  normals->reserve(vertices->size());

  // normals for top
  for (int i = 0; i < m_slice + 2; ++i)
  {
    normals->push_back(vec3(0, 0, 1));
  }

  // normals for bottom
  for (int i = 0; i < m_slice + 2; ++i)
  {
    normals->push_back(vec3(0, 0, -1));
  }

  vec3 apex(0, 0, m_height);

  // normals for 1st stack
  for (int i = 0; i <= m_slice; ++i)
  {
    const vec3& vertex = (*vertices)[vertices->size() - (m_slice + 1) + i];
    vec3 outer = cross(vertex, apex);
    vec3 normal = cross(apex - vertex, outer);
    normals->push_back(normal);
  }

  // normals for other stacks
  auto stack_start = 2 * (m_slice + 2);
  for (int i = 1; i <= m_stack; ++i)
  {
    // no reallocate will happen here
    normals->insert(normals->end(), normals->begin() + stack_start,
      normals->begin() + stack_start + m_slice + 1);
  }

  assert(normals->size() == num_vertices());
}

//--------------------------------------------------------------------
void cylinder::build_texcoord()
{
  auto texcoords = make_array<vec2_array>(num_arrays());
  texcoords->reserve(num_vertices());

  // top tex coords
  texcoords->push_back(vec2(0, 1));
  for (int i = 0; i <= m_slice; ++i)
  {
    GLfloat s = static_cast<GLfloat>(i) / m_slice;
    texcoords->push_back(vec2(s, 1));
  }

  // bottom texcoords
  texcoords->push_back(vec2(0, 0));
  for (int i = 0; i <= m_slice; ++i)
  {
    GLfloat s = 1 - static_cast<GLfloat>(i) / m_slice;
    texcoords->push_back(vec2(s, 0));
  }

  for (int i = 0; i <= m_stack; ++i)
  {
    GLfloat t = static_cast<GLfloat>(i) / m_stack;
    for (int j = 0; j <= m_slice; j++)
    {
      GLfloat s = static_cast<GLfloat>(j) / m_slice;
      texcoords->push_back(vec2(s, t));
    }
  }

  assert(texcoords->size() == num_vertices());
}

void cylinder::on_draw()
{
  glEnable(GL_PRIMITIVE_RESTART_FIXED_INDEX);
}

}
