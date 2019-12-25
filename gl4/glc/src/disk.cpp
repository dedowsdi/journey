#include <disk.h>

#include <algorithm>

#include <glmath.h>
#include <geometry_util.h>
#include <glm/trigonometric.hpp>

namespace zxd
{

//--------------------------------------------------------------------
disk::disk(GLfloat inner, GLfloat outer, GLuint slice, GLuint loop,
  GLfloat start, GLfloat sweep)
    : m_inner(inner), m_outer(outer), m_slice(slice), m_loop(loop),
      m_start(start), m_sweep(sweep)
{
}

//--------------------------------------------------------------------
common_geometry::vertex_build disk::build_vertices()
{
  auto vertices = std::make_shared<vec2_array>();
  auto num_vertices = (m_loop + 1) * (m_slice + 1);
  vertices->reserve(num_vertices);

  GLfloat theta_step = m_sweep / m_slice;
  GLfloat radius_step = (m_outer - m_inner) / m_loop;

  for (auto i = 0u; i <= m_loop; ++i)
  {
    GLfloat radius = m_inner + radius_step * i;
    for (auto j = 0; j <= m_slice; ++j)
    {
      GLfloat theta =
        m_sweep == f2pi && j == m_slice ? 0 : m_start + theta_step * j;
      vertices->push_back(vec2(radius * cos(theta), radius * sin(theta)));
    }
  }

  assert(vertices->size() == num_vertices);

  auto elements = make_element<uint_array>();
  auto num_elemnets = m_loop * 2 * (m_slice + 2);
  build_strip_elements(elements, m_loop, m_slice);

  clear_primitive_sets();
  add_primitive_set(
    std::make_shared<draw_elements>(GL_TRIANGLE_STRIP, elements->size(), GL_UNSIGNED_INT, 0));
  return vertex_build{vertices};
}

//--------------------------------------------------------------------
array_ptr disk::build_normals(const array& vertices)
{
  auto normals = std::make_unique<vec3_array>();
  normals->resize(vertices.size(), vec3(0, 0, 1));
  return normals;
}

//--------------------------------------------------------------------
array_ptr disk::build_texcoords(const array& vertices)
{
  auto texcoords = std::make_unique<vec2_array>();
  texcoords->reserve(vertices.size());

  auto& v = static_cast<const vec2_array&>(vertices);
  std::transform(v.begin(), v.end(), std::back_inserter(*texcoords),
    [this](const auto& v) -> vec2 { return vec2(v) * 0.5f / m_outer + 0.5f; });
  return texcoords;
}

}
