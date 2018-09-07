#ifndef GEOMETRY_H
#define GEOMETRY_H
#include "gl.h"
#include "glm.h"
#include "common.h"
#include "array.h"
#include <memory>

namespace zxd {

typedef std::vector<array_ptr> array_vector;

class geometry_base {
protected:
  GLuint m_vao;
  array_vector m_attributes;

public:
  geometry_base() : m_vao(-1) {}
  virtual ~geometry_base() {}

  void attrib_array(GLuint index, array_ptr _array);
  array_ptr attrib_array(GLuint index);
  float_array_ptr attrib_float_array(GLuint index);
  vec2_array_ptr attrib_vec2_array(GLuint index);
  vec3_array_ptr attrib_vec3_array(GLuint index);
  vec4_array_ptr attrib_vec4_array(GLuint index);

  virtual GLuint num_vertices();
  geometry_base& bind(GLint attrib_index, GLint attrib_location);
  // the most common case, assume vertex 0, normal 1, texcoord 2
  geometry_base& bind(GLint vertex, GLint normal, GLint texcoord);
  geometry_base& bind_vntt(GLint vertex, GLint normal, GLint texcoord, GLint tangent);
  geometry_base& bind_vc(GLint vertex, GLint color);

  GLuint num_arrays() { return m_attributes.size(); }

  virtual void draw(GLuint primcount = 1) = 0;

  // a wrapper of glDrawArrays and glDrawArraysInstanced[ARB]
  void draw_arrays(
    GLenum mode, GLint first, GLsizei count, GLsizei primcount) const;

  void bind_vao();

  virtual void build_mesh();
  virtual GLint build_vertex() { return -1; }
  virtual GLint build_color() { return -1; }
  virtual GLint build_normal() { return -1; }
  virtual GLint build_texcoord() { return -1; }
  virtual GLint build_tangent() { return -1; }

  GLuint vao() const { return m_vao; }
};
}

#endif /* GEOMETRY_H */
