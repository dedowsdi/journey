#ifndef GL_GLC_GEOMETRY_H
#define GL_GLC_GEOMETRY_H

#include "gl.h"

#include <glm/fwd.hpp>
#include "array.h"
#include "primitive_set.h"

namespace zxd
{

class array;
typedef std::shared_ptr<array> array_ptr;
typedef std::vector<array_ptr> array_vector;

class geometry_base
{
public:
  geometry_base(){}
  virtual ~geometry_base() {}

  template <typename T>
  std::shared_ptr<T> make_array(GLuint index)
  {
    auto arr = std::make_shared<T>();
    attrib_array(index, arr);
    return arr;
  }

  void attrib_array(GLuint index, array_ptr _array);
  array_ptr attrib_array(GLuint index) const;
  float_array_ptr attrib_float_array(GLuint index) const;
  vec2_array_ptr attrib_vec2_array(GLuint index) const;
  vec3_array_ptr attrib_vec3_array(GLuint index) const;
  vec4_array_ptr attrib_vec4_array(GLuint index) const;

  virtual GLuint num_vertices();

  GLuint num_arrays() { return m_attributes.size(); }

  virtual void draw();

  void bind_vao();

  void clear();
  virtual void build_mesh();
  virtual void build_vertex() {}
  virtual void build_color() {}
  virtual void build_normal() {}
  virtual void build_texcoord() {}
  virtual void build_tangent() {}

  GLuint vao() const { return m_vao; }
  bool is_inited() const { return m_vao != -1;}

  GLboolean include_normal() const { return m_include_normal; }
  void include_normal(GLboolean v){ m_include_normal = v; }

  GLboolean include_color() const { return m_include_color; }
  void include_color(GLboolean v){ m_include_color = v; }

  GLboolean include_texcoord() const { return m_include_texcoord; }
  void include_texcoord(GLboolean v){ m_include_texcoord = v; }

  GLboolean include_tangent() const { return m_include_tangent; }
  void include_tangent(GLboolean v){ m_include_tangent = v; }

  void bind_and_update_buffer();
  void bind_and_update_buffer(GLint index);
  void rebind(GLint array_index, GLint attrib_index);

  primitive_set* get_primitive_set(GLuint index);
  GLuint get_num_primitive_set();
  void add_primitive_set(primitive_set* ps);
  void clear_primitive_sets();
  void remove_primitive_sets(GLuint index, GLuint count);
  void set_num_instance(GLuint count);

  virtual void accept(primitive_functor& pf) const;

  array_ptr elements() const { return m_elements; }
  void elements(array_ptr v){ m_elements = v; }

private:
  virtual void on_draw() {}

protected:
  GLuint m_vao = -1;
  array_vector m_attributes;
  array_ptr m_elements;

  GLboolean m_include_normal = GL_FALSE;
  GLboolean m_include_color = GL_FALSE;
  GLboolean m_include_texcoord = GL_FALSE;
  GLboolean m_include_tangent = GL_FALSE;
  primitive_set_vector m_primitive_sets;


};
}

#endif /* GL_GLC_GEOMETRY_H */
