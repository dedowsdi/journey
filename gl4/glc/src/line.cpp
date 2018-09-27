#include "line.h"

namespace zxd
{

//--------------------------------------------------------------------
line::line(GLenum mode):
  m_mode(mode)
{

}

//--------------------------------------------------------------------
void line::build_mesh(const vec3& v0, const vec3& v1, const vec4& color)
{
  build_mesh(v0, v1);
  vec4_array* colors = new vec4_array;

  colors->push_back(color);
  colors->push_back(color);
  colors->update_array_buffer();

  attrib_array(1, array_ptr(colors));
}

//--------------------------------------------------------------------
void line::build_mesh(const vec3& v0, const vec3& v1)
{
  vec3_array* vertices = new vec3_array;

  vertices->reserve(2);
  vertices->push_back(v0);
  vertices->push_back(v1);
  vertices->update_array_buffer();

  attrib_array(0, array_ptr(vertices));
}

//--------------------------------------------------------------------
void line::build_mesh(const vec2& v0, const vec2& v1, const vec4& color)
{
  build_mesh(v0, v1);
  vec4_array* colors = new vec4_array;

  colors->push_back(color);
  colors->push_back(color);
  colors->update_array_buffer();

  attrib_array(1, array_ptr(colors));
}

//--------------------------------------------------------------------
void line::build_mesh(const vec2& v0, const vec2& v1)
{
  vec2_array* vertices = new vec2_array;

  vertices->reserve(2);
  vertices->push_back(v0);
  vertices->push_back(v1);
  vertices->update_array_buffer();

  attrib_array(0, array_ptr(vertices));
}

//--------------------------------------------------------------------
void line::draw(GLuint primcount/* = -1*/)
{
  bind_vao();
  draw_arrays(m_mode, 0, attrib_array(0)->num_elements(), primcount);
}
}
