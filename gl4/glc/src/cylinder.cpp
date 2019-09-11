#include <cylinder.h>

#include <algorithm>

#include <common.h>
#include <geometry_util.h>

namespace zxd
{

cylinder::cylinder(
  GLfloat base, GLfloat top, GLfloat height, GLuint slice, GLuint stack)
    : m_base(base), m_top(top), m_height(height), m_slice(slice), m_stack(stack)
{
}

//--------------------------------------------------------------------
common_geometry::vertex_build cylinder::build_vertices()
{
                               // create vertices

  auto vertices = std::make_unique<vec3_array>();
  auto num_vertices = 2 * (m_slice + 2) + (m_stack + 1) * (m_slice + 1);

  vertices->reserve(num_vertices);

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

  assert(vertices->size() == num_vertices);

                         // create elements for stacks

  auto& elements = make_element<uint_array>();

  auto num_elements = m_stack * 2 * (m_slice + 1) + m_stack;
  elements.reserve(num_elements);
  build_strip_elements(elements, m_stack, m_slice, 2 * (m_slice + 2));
  assert(elements.size() == num_elements);

  clear_primitive_sets();

  add_primitive_set(std::make_shared<draw_arrays>(GL_TRIANGLE_FAN, 0, m_slice + 2));
  add_primitive_set(std::make_shared<draw_arrays>(GL_TRIANGLE_FAN, m_slice + 2, m_slice + 2));
  add_primitive_set(
    std::make_unique<draw_elements>(GL_TRIANGLE_STRIP, elements.size(), GL_UNSIGNED_INT, 0));
  return vertex_build{std::move(vertices)};
}

//--------------------------------------------------------------------
array_uptr cylinder::build_normals(const array& vertices)
{
  auto normals = std::make_unique<vec3_array>();
  normals->reserve(vertices.size());

  // normals for top
  normals->insert(normals->end(), m_slice + 2, vec3(0, 0, 1));

  // normals for bottom
  normals->insert(normals->end(), m_slice + 2, vec3(0, 0, -1));

  vec3 apex(0, 0, m_height);

  auto& v = static_cast<const vec3_array&>(vertices);
  // normals for 1st stack
  for (int i = 0; i <= m_slice; ++i)
  {
    const vec3& vertex = v[v.size() - (m_slice + 1) + i];
    vec3 outer = cross(vertex, apex);
    vec3 normal = cross(apex - vertex, outer);
    normals->push_back(normal);
  }

  // normals for other stacks
  auto stack_start = 2 * (m_slice + 2);
  for (int i = 1; i <= m_stack; ++i)
  {
    normals->insert(normals->end(), normals->begin() + stack_start,
      normals->begin() + stack_start + m_slice + 1);
  }

  assert(normals->size() == vertices.size());
  return normals;
}

//--------------------------------------------------------------------
array_uptr cylinder::build_texcoords(const array& vertices)
{
  auto texcoords = std::make_unique<vec2_array>();
  texcoords->reserve(vertices.size());

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

  // cylinder texcoords
  build_strip_texcoords(*texcoords, m_stack, m_slice, 1, 0);

  assert(texcoords->size() == vertices.size());
  return texcoords;
}

}
