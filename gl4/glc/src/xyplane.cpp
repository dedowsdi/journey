#include <xyplane.h>

#include <geometry_util.h>

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
common_geometry::vertex_build xyplane::build_vertices()
{
  auto vertices = std::make_unique<vec2_array>();
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

  auto& elements = make_element<uint_array>();
  build_strip_elements(elements, m_slice, m_slice);

  clear_primitive_sets();

  add_primitive_set(std::make_shared<draw_elements>(
    GL_TRIANGLE_STRIP, elements.size(), GL_UNSIGNED_INT, 0));
  return vertex_build{std::move(vertices)};
}

//--------------------------------------------------------------------
array_uptr xyplane::build_normals(const array& vertices)
{
  auto normals = std::make_unique<vec3_array>();
  normals->resize(vertices.size(), vec3(0, 0, 1));
  return normals;
}

//--------------------------------------------------------------------
array_uptr xyplane::build_texcoords(const array& vertices)
{
  auto texcoords = std::make_unique<vec2_array>();
  texcoords->reserve(vertices.size());
  build_strip_texcoords(*texcoords, m_slice, m_slice, 1, 0);
  assert(texcoords->size() == vertices.size());
  return texcoords;
}

}
