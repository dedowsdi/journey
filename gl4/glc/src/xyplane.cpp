#include <xyplane.h>

#include <geometry_util.h>

namespace zxd
{

//--------------------------------------------------------------------
xyplane::xyplane(GLfloat width, GLfloat height, GLuint slice):
  xyplane(-width * 0.5f, -height * 0.5f, width * 0.5f, height * 0.5f, slice)
{
}

//--------------------------------------------------------------------
xyplane::xyplane(GLfloat x0, GLfloat y0, GLfloat x1, GLfloat y1, GLuint slice):
  m_point0(x0, y0),
  m_point1(x1, y1),
  m_slice(slice)
{
}

//--------------------------------------------------------------------
void xyplane::set_texcoord(const vec2& texcoord0, const vec2& texcoord1)
{
  m_texcoord0 = texcoord0;
  m_texcoord1 = texcoord1;
}

//--------------------------------------------------------------------
common_geometry::vertex_build xyplane::build_vertices()
{
  auto vertices = std::make_unique<vec2_array>();
  auto num_vertices = (m_slice + 1) * (m_slice + 1);
  vertices->reserve(num_vertices);

  auto xstep = (m_point1.x - m_point0.x) / m_slice;
  auto ystep = (m_point1.y - m_point0.y) / m_slice;

  for (auto i = 0u; i <= m_slice; ++i)
  {
    auto y = m_point0.y + ystep * i;
    for (auto j = 0u; j <= m_slice; ++j)
    {
      auto x = m_point0.x  + xstep * j;
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
  build_strip_texcoords(*texcoords, m_slice, m_slice, m_texcoord0, m_texcoord1);
  assert(texcoords->size() == vertices.size());
  return texcoords;
}

}
