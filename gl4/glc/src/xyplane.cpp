#include "xyplane.h"

#include "geometry_util.h"

namespace zxd
{

//--------------------------------------------------------------------
xyplane::xyplane(GLfloat width, GLfloat height, GLuint slice)
    : m_width(width), m_height(height), m_slice(slice), m_left(-width * 0.5),
      m_bottom(-height * 0.5)
{
}

//--------------------------------------------------------------------
xyplane::xyplane(GLfloat x0, GLfloat y0, GLfloat x1, GLfloat y1, GLuint slice)
    : xyplane(x1 - x0, y1 - y0, slice)
{
}

//--------------------------------------------------------------------
void xyplane::build_vertex()
{
  auto vertices = make_array<vec2_array>(0);
  auto num_vertices = (m_slice + 1) * (m_slice + 1);
  vertices->reserve(num_vertices);

  auto xstep = m_width / m_slice;
  auto ystep = m_height / m_slice;

  for (auto i = 0u; i <= m_slice; ++i)
  {
    auto y = m_bottom + m_height - ystep * i;
    for (auto j = 0u; j <= m_slice; ++j)
    {
      auto x = m_left + xstep * j;
      vertices->push_back(vec2(x, y));
    }
  }

  assert(vertices->size() == num_vertices);

  auto elements = make_element<uint_array>();
  build_strip_elements(*elements, m_slice, m_slice);

  m_primitive_sets.clear();

  add_primitive_set(
    new draw_elements(GL_TRIANGLE_STRIP, elements->size(), GL_UNSIGNED_INT, 0));
}

//--------------------------------------------------------------------
void xyplane::build_normal()
{
  auto normals = make_array<vec3_array>(num_arrays());
  normals->resize(num_vertices(), vec3(0, 0, 1));
}

//--------------------------------------------------------------------
void xyplane::build_texcoord()
{
  auto texcoords = make_array<vec2_array>(num_arrays());
  texcoords->reserve(num_vertices());
  build_strip_texcoords(*texcoords, m_slice, m_slice, 1, 0);
  assert(texcoords->size() == num_vertices());
}

}
