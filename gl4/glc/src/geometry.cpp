#include "geometry.h"
#include "common.h"

namespace zxd {

//--------------------------------------------------------------------
void geometry_base::attrib_array(GLuint index, array_ptr _array) {
  if (index >= m_attributes.size()) {
    m_attributes.resize(index + 1);
  }
  m_attributes[index] = _array;
}

//--------------------------------------------------------------------
array_ptr geometry_base::attrib_array(GLuint index) {
  if (index >= m_attributes.size()) {
    std::cerr << " attribute index overflow : " << index << std::endl;
  }
  return m_attributes[index];
}

//--------------------------------------------------------------------
float_array_ptr geometry_base::attrib_float_array(GLuint index) {
  return std::static_pointer_cast<float_array>(attrib_array(index));
}

//--------------------------------------------------------------------
vec2_array_ptr geometry_base::attrib_vec2_array(GLuint index) {
  return std::static_pointer_cast<vec2_array>(attrib_array(index));
}

//--------------------------------------------------------------------
vec3_array_ptr geometry_base::attrib_vec3_array(GLuint index) {
  return std::static_pointer_cast<vec3_array>(attrib_array(index));
}

//--------------------------------------------------------------------
vec4_array_ptr geometry_base::attrib_vec4_array(GLuint index) {
  return std::static_pointer_cast<vec4_array>(attrib_array(index));
}

//--------------------------------------------------------------------
GLuint geometry_base::num_vertices() { return attrib_array(0)->num_elements(); }

//--------------------------------------------------------------------
void geometry_base::build_mesh() {
  build_vertex();

  if(m_include_color)
  {
    build_color();
  }

  if(m_include_normal)
  {
    build_normal();
  }

  if(m_include_texcoord)
  {
    build_texcoord();
  }

  if(m_include_tangent)
  {
    build_tangent();
  }

  bind_and_update_buffer();
}

//--------------------------------------------------------------------
void geometry_base::bind_and_update_buffer()
{
  bind_vao();
  for (int i = 0; i < m_attributes.size(); ++i) {
    m_attributes[i]->bind(i);
    m_attributes[i]->update_array_buffer();
  }
}

//--------------------------------------------------------------------
void geometry_base::draw_arrays(
  GLenum mode, GLint first, GLsizei count, GLsizei primcount) const {
  if (primcount == -1) {
    glDrawArrays(mode, first, count);
  } else {
#ifdef GL_VERSION_3_0
    glDrawArraysInstanced(mode, first, count, primcount);
#else
    glDrawArraysInstancedARB(mode, first, count, primcount);
#endif
  }
}

//--------------------------------------------------------------------
void geometry_base::bind_vao() {
  if (m_vao == -1) {
    glGenVertexArrays(1, &m_vao);
  }
  glBindVertexArray(m_vao);
}
}
