#include "disk.h"

#include <algorithm>

#include "glmath.h"
#include "geometry_util.h"

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
void disk::build_vertex()
{
  auto vertices = make_array<vec3_array>(0);
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
      vertices->push_back(vec3(radius * cos(theta), radius * sin(theta), 0));
    }
  }

  assert(vertices->size() == num_vertices);

  auto elements = make_element<uint_array>();
  auto num_elemnets = m_loop * 2 * (m_slice + 2);
  build_strip_elements(*elements, m_loop, m_slice);

  m_primitive_sets.clear();
  add_primitive_set(
    new draw_elements(GL_TRIANGLE_STRIP, elements->size(), GL_UNSIGNED_INT, 0));
}

//--------------------------------------------------------------------
void disk::build_normal()
{
  auto normals = make_array<vec3_array>(num_arrays());
  normals->resize(num_vertices(), vec3(0, 0, 1));
}

//--------------------------------------------------------------------
void disk::build_texcoord()
{
  auto texcoords = make_array<vec2_array>(num_arrays());
  texcoords->reserve(num_vertices());
  auto vertices = attrib_vec3_array(0);

  std::transform(vertices->begin(), vertices->end(),
    std::back_inserter(*texcoords),
    [this](const auto& v) -> vec2 { return vec2(v) * 0.5f / m_outer + 0.5f; });
}

}
