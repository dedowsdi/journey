#ifndef GL4_GLC_GEOMETRY_H
#define GL4_GLC_GEOMETRY_H

#include <map>
#include <memory>

#include <primitive_set.h>
#include <array.h>

namespace zxd
{

class vao;
class buffer;

using primitive_sets = std::vector<std::shared_ptr<primitive_set>>;

class geometry_base
{
public:

  virtual ~geometry_base() = default;

  void draw();
  void set_num_instance(GLuint count);

  const vao& get_vao() const { return *_vao; }
  vao& get_vao(){ return *_vao; }
  void set_vao(std::shared_ptr<vao> vao_){ _vao = std::move(vao_); }
  vao& get_or_create_vao();

  bool is_inited() { return static_cast<bool>(_vao);}

  template <typename T>
  T& make_element();

  const array* get_attrib_array(GLuint index) const;
  const vec2_array* get_attrib_vec2_array(GLuint index) const;
  const vec3_array* get_attrib_vec3_array(GLuint index) const;
  const vec4_array* get_attrib_vec4_array(GLuint index) const;

  array* get_attrib_array(GLuint index);
  vec2_array* get_attrib_vec2_array(GLuint index);
  vec3_array* get_attrib_vec3_array(GLuint index);
  vec4_array* get_attrib_vec4_array(GLuint index);

  void set_attrib_array(GLuint index, std::unique_ptr<array> a);

  void set_element_array(std::unique_ptr<array> a);

  GLuint num_vertices() const;

  const primitive_set& get_primitive_set(GLuint index) const;
  primitive_set& get_primitive_set(GLuint index);
  GLuint get_num_primitive_set() const;
  void add_primitive_set(std::shared_ptr<primitive_set> ps);
  void clear_primitive_sets();
  void remove_primitive_sets(GLuint index, GLuint count);

  // only works for vec3_array
  virtual void accept(primitive_functor& pf) const;

  const std::shared_ptr<buffer>& get_element_buffer() const { return _element_buffer; }
  std::shared_ptr<buffer>& get_element_buffer() { return _element_buffer; }
  void set_element_buffer(const std::shared_ptr<buffer>& v){ _element_buffer = v; }

  virtual void build() {};

protected:
  std::shared_ptr<vao> _vao;

private:

  virtual void on_draw() {}

  std::shared_ptr<buffer> _element_buffer;
  primitive_sets _primitive_sets;

};

enum class attrib_semantic
{
  vertex,
  normal,
  texcoord,
  color,
  tangent,
};

class common_geometry : public geometry_base
{
public:

  enum class vertex_style
  {
    array_of_struct,
    struct_of_array
  };

  common_geometry();

  void build_mesh(
    std::initializer_list<attrib_semantic> list = {attrib_semantic::vertex});

  vertex_style get_vertex_style() const { return _vertex_style; }
  void set_vertex_style(vertex_style v){ _vertex_style = v; }

  bool has(attrib_semantic type);
  bool has_vertex() { return has(attrib_semantic::vertex); }
  bool has_normal() { return has(attrib_semantic::normal); }
  bool has_texcoord() { return has(attrib_semantic::texcoord); }
  bool has_color() { return has(attrib_semantic::color); }
  bool has_tangent() { return has(attrib_semantic::tangent); }

protected:
  struct vertex_build
  {
    vertex_build()
    {
    }

    vertex_build(array_uptr ptr)
    {
      am.insert(std::make_pair(attrib_semantic::vertex, std::move(ptr)));
    }

    vertex_build(std::map<attrib_semantic, array_uptr>&& m) : am(std::move(m)) {}

    std::map<attrib_semantic, array_uptr> am;
  };

private:

  virtual vertex_build build_vertices() { return array_uptr{}; };
  virtual array_uptr build_normals(const array& vertices) { return array_uptr{}; };
  virtual array_uptr build_texcoords(const array& vertices) { return array_uptr{}; };
  virtual array_uptr build_colors(const array& vertices);
  virtual array_uptr build_tangents(const array& vertices) { return array_uptr{}; };

  vertex_style _vertex_style {vertex_style::struct_of_array};
  std::vector<attrib_semantic> _attrib_semantics;

};

}

#endif /* GL4_GLC_GEOMETRY_H */
