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
  GLuint m_vao = -1;
  array_vector m_attributes;

  GLboolean m_include_normal = GL_FALSE;
  GLboolean m_include_color = GL_FALSE;
  GLboolean m_include_texcoord = GL_FALSE;
  GLboolean m_include_tangent = GL_FALSE;

public:
  geometry_base(){}
  virtual ~geometry_base() {}

  void attrib_array(GLuint index, array_ptr _array);
  array_ptr attrib_array(GLuint index);
  float_array_ptr attrib_float_array(GLuint index);
  vec2_array_ptr attrib_vec2_array(GLuint index);
  vec3_array_ptr attrib_vec3_array(GLuint index);
  vec4_array_ptr attrib_vec4_array(GLuint index);

  virtual GLuint num_vertices();

  GLuint num_arrays() { return m_attributes.size(); }

  virtual void draw(GLuint primcount = -1) = 0;

  // a wrapper of glDrawArrays and glDrawArraysInstanced[ARB]
  void draw_arrays(
    GLenum mode, GLint first, GLsizei count, GLsizei primcount) const;

  void bind_vao();

  virtual void build_mesh();
  virtual void build_vertex() {}
  virtual void build_color() {}
  virtual void build_normal() {}
  virtual void build_texcoord() {}
  virtual void build_tangent() {}

  GLuint vao() const { return m_vao; }

  GLboolean include_normal() const { return m_include_normal; }
  void include_normal(GLboolean v){ m_include_normal = v; }

  GLboolean include_color() const { return m_include_color; }
  void include_color(GLboolean v){ m_include_color = v; }

  GLboolean include_texcoord() const { return m_include_texcoord; }
  void include_texcoord(GLboolean v){ m_include_texcoord = v; }

  GLboolean include_tangent() const { return m_include_tangent; }
  void include_tangent(GLboolean v){ m_include_tangent = v; }

  void bind_and_update_buffer();

};
}

#endif /* GEOMETRY_H */
