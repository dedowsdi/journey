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
geometry_base& geometry_base::bind(GLint attrib_index, GLint attrib_location) {
  array_ptr _array = attrib_array(attrib_index);
  if (!_array) {
    std::cerr << attrib_index << " is not a valid vertex attrib array index"
              << std::endl;
  }
  _array->bind(attrib_location);
  return *this;
}

//--------------------------------------------------------------------
geometry_base& geometry_base::bind(GLint vertex, GLint normal, GLint texcoord) {
  bind_vao();
  GLint idx = 0;

  if (vertex != -1) bind(idx++, vertex);
  if (normal != -1) bind(idx++, normal);
  if (texcoord != -1) bind(idx++, texcoord);

  return *this;
}

//--------------------------------------------------------------------
geometry_base& geometry_base::bind_vntt(
  GLint vertex, GLint normal, GLint texcoord, GLint tangent) {
  bind_vao();
  GLint idx = 0;

  if (vertex != -1) bind(idx++, vertex);
  if (normal != -1) bind(idx++, normal);
  if (texcoord != -1) bind(idx++, texcoord);
  if (tangent != -1) bind(idx++, tangent);

  return *this;
}

//--------------------------------------------------------------------
geometry_base& geometry_base::bind_vn(GLint vertex, GLint normal)
{
  bind_vao();
  GLint idx = 0;
  if (vertex != -1) bind(idx++, vertex);
  if (normal != -1) bind(idx++, normal);
  return *this;
}

//--------------------------------------------------------------------
geometry_base& geometry_base::bind_vc(GLint vertex, GLint color) {
  bind_vao();
  GLint idx = 0;

  if (vertex != -1) bind(idx++, vertex);
  if (color != -1) bind(idx++, color);

  return *this;
}

//--------------------------------------------------------------------
void geometry_base::build_mesh() {
  GLint idx;
  idx = build_vertex();
  if (idx != -1) attrib_array(idx)->update_array_buffer();

  idx = build_color();
  if (idx != -1) attrib_array(idx)->update_array_buffer();

  idx = build_normal();
  if (idx != -1) attrib_array(idx)->update_array_buffer();

  idx = build_texcoord();
  if (idx != -1) attrib_array(idx)->update_array_buffer();

  idx = build_tangent();
  if (idx != -1) attrib_array(idx)->update_array_buffer();
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
