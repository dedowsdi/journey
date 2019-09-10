#include "geometry.h"

#include <iostream>
#include <algorithm>

#include <exception.h>
#include <buffer.h>
#include <array.h>
#include <glm/gtc/random.hpp>
#include <stream_util.h>
#include <geometry_util.h>

namespace zxd
{

//--------------------------------------------------------------------
void geometry_base::draw()
{
  on_draw();

  _vao->bind();
  if (_element_buffer)
  {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _element_buffer->get_object());
    glEnable(GL_PRIMITIVE_RESTART_FIXED_INDEX);
  }

  for (auto& s : _primitive_sets)
  {
    s->draw();
  }
}

//--------------------------------------------------------------------
void geometry_base::set_num_instance(GLuint count)
{
  for (auto& s : _primitive_sets)
  {
    s->num_instance(count);
  }
}

//--------------------------------------------------------------------
vao& geometry_base::get_or_create_vao()
{
  if (!_vao)
    _vao = std::make_shared<vao>();

  return *_vao;
}

//--------------------------------------------------------------------
const array* geometry_base::get_attrib_array(GLuint index)  const
{
  return _vao->get_attrib(index).buf->get_data<std::unique_ptr<array>>().get();
}

//--------------------------------------------------------------------
const vec2_array* geometry_base::get_attrib_vec2_array(GLuint index)  const
{
  return dynamic_cast<const vec2_array*>(get_attrib_array(index));
}

//--------------------------------------------------------------------
const vec3_array* geometry_base::get_attrib_vec3_array(GLuint index)  const
{
  return dynamic_cast<const vec3_array*>(get_attrib_array(index));
}

//--------------------------------------------------------------------
const vec4_array* geometry_base::get_attrib_vec4_array(GLuint index)  const
{
  return dynamic_cast<const vec4_array*>(get_attrib_array(index));
}

//--------------------------------------------------------------------
array* geometry_base::get_attrib_array(GLuint index)
{
  return _vao->get_attrib(index).buf->get_data<std::unique_ptr<array>>().get();
}

//--------------------------------------------------------------------
vec2_array* geometry_base::get_attrib_vec2_array(GLuint index)
{
  return dynamic_cast<vec2_array*>(get_attrib_array(index));
}

//--------------------------------------------------------------------
vec3_array* geometry_base::get_attrib_vec3_array(GLuint index)
{
  return dynamic_cast<vec3_array*>(get_attrib_array(index));
}

//--------------------------------------------------------------------
vec4_array* geometry_base::get_attrib_vec4_array(GLuint index)
{
  return dynamic_cast<vec4_array*>(get_attrib_array(index));
}

//--------------------------------------------------------------------
void geometry_base::set_attrib_array(GLuint index, std::unique_ptr<array> a)
{
  add_array_attrib(get_or_create_vao(), index, std::move(a));
}

//--------------------------------------------------------------------
void geometry_base::set_element_array(std::unique_ptr<array> a)
{
  _element_buffer = std::make_shared<buffer>();
  _element_buffer->bind(GL_ELEMENT_ARRAY_BUFFER);
  _element_buffer->buffer_data(std::move(a), GL_STATIC_DRAW);
}

//--------------------------------------------------------------------
GLuint geometry_base::num_vertices() const
{
  return get_attrib_array(0)->size();
}

//--------------------------------------------------------------------
const primitive_set& geometry_base::get_primitive_set(GLuint index) const
{
  if(index >= _primitive_sets.size())
    throw gl_overflow("primitive set index over flow");
  return *_primitive_sets[index];
}

//--------------------------------------------------------------------
primitive_set& geometry_base::get_primitive_set(GLuint index)
{
  if(index >= _primitive_sets.size())
    throw gl_overflow("primitive set index over flow");
  return *_primitive_sets[index];
}

//--------------------------------------------------------------------
GLuint geometry_base::get_num_primitive_set() const
{
  return _primitive_sets.size();
}

//--------------------------------------------------------------------
void geometry_base::add_primitive_set(std::shared_ptr<primitive_set> ps)
{
  // this might cause problem, i should use shared_ptr as argument!!!
  _primitive_sets.push_back(std::move(ps));
}

//--------------------------------------------------------------------
void geometry_base::clear_primitive_sets()
{
  _primitive_sets.clear();
}

//--------------------------------------------------------------------
void geometry_base::remove_primitive_sets(GLuint index, GLuint count)
{
  if (index + count > _primitive_sets.size())
    throw gl_overflow("error, too many remove count");
  _primitive_sets.erase(
    _primitive_sets.begin() + index, _primitive_sets.begin() + index + count);
}

//--------------------------------------------------------------------
void geometry_base::accept(primitive_functor& pf) const
{
  auto a = _vao->get_attrib(0).buf->get_data<std::unique_ptr<array>>().get();
  auto vertices = get_attrib_vec3_array(0);

  pf.set_vertex_array( vertices->size(), &vertices->front());
  for(const auto& item : _primitive_sets)
  {
    item->accept(pf);
  }
}

//--------------------------------------------------------------------
common_geometry::common_geometry()
{
}

//--------------------------------------------------------------------
void common_geometry::build_mesh(std::initializer_list<attrib_semantic> list)
{
  _attrib_semantics.assign(list.begin(), list.end());

  if (!has_vertex())
  {
    throw gl_illegal_vertex_specification("missing vertex");
  }

  get_element_buffer().reset();
  _vao = std::make_unique<vao>();
  _vao->bind();

  auto am = build_vertices().am;

  auto& vertices = *am[attrib_semantic::vertex];
  if (has_normal() && am.find(attrib_semantic::normal) == am.end())
  {
    if (auto arr = build_normals(vertices))
    {
      am.insert(std::make_pair(attrib_semantic::normal, std::move(arr)));
    }
    else
    {
      // create smooth normal if it doesn't exists
      add_array_attrib(*_vao, 0, vertices);
      auto normals = std::make_unique<vec3_array>(get_smooth_normal(*this));
      assert(vertices.size() == normals->size());
      am.insert(std::make_pair(attrib_semantic::normal, std::move(normals)));
      std::cout << "smoothing normals for geometry " << this <<  std::endl;
    }
  }

  if (has_texcoord() && am.find(attrib_semantic::texcoord) == am.end())
  {
    auto arr = build_texcoords(vertices);
    if (!arr)
      throw gl_geometry_error("missing texcoords");
    am.insert(std::make_pair(attrib_semantic::texcoord, std::move(arr)));
  }

  if (has_color() && am.find(attrib_semantic::color) == am.end())
  {
    auto arr = build_colors(vertices);
    if (!arr)
      throw gl_geometry_error("missing colorss");
    am.insert(std::make_pair(attrib_semantic::color, std::move(arr)));
  }

  if (has_tangent() && am.find(attrib_semantic::tangent) == am.end())
  {
    auto arr = build_tangents(vertices);
    if (!arr)
      throw gl_geometry_error("missing tangentss");
    am.insert(std::make_pair(attrib_semantic::tangent, std::move(arr)));
  }

  auto ebuf = get_element_buffer();
  if (ebuf)
  {
    ebuf->bind(GL_ELEMENT_ARRAY_BUFFER);
    auto& buf_data = ebuf->get_data<std::unique_ptr<array>>();
    ebuf->buffer_data(buf_data->bytes(), buf_data->data(), GL_STATIC_DRAW);
  }

  auto num_vertices = vertices.size();

  if (_vertex_style == vertex_style::struct_of_array)
  {
    for (auto i = 0; i < _attrib_semantics.size(); ++i)
    {
      auto type = _attrib_semantics[i];
      assert(num_vertices == am[type]->size());
      add_array_attrib(*_vao, i, std::move(am[type]));
    }
  }
  else
  {
    // calculate vertex size and total buffer size
    auto vertex_size = 0;
    auto total_bytes = 0;
    for (auto type : _attrib_semantics)
    {
      const auto& array = am[type];
      assert(num_vertices == array->size());
      vertex_size += array->element_bytes();
      total_bytes += array->bytes();
    }

    // populate buffer
    auto buf = std::make_shared<buffer>();
    buf->bind(GL_ARRAY_BUFFER);
    auto data = std::make_unique<GLchar[]>(total_bytes);
    auto p = data.get();
    for (auto i = 0; i < num_vertices; ++i)
    {
      for (auto j = 0; j < _attrib_semantics.size(); ++j)
      {
        const auto& array = am[_attrib_semantics[j]];
        auto elem = static_cast<const GLchar*>(array->data()) +
                    array->element_bytes() * i;
        std::copy(elem, elem + array->element_bytes(), p);
        p += array->element_bytes();
      }
    }

    // create vertex attribs
    GLuint offset = 0;
    for (auto i = 0; i < _attrib_semantics.size(); ++i)
    {
      const auto& array = am[_attrib_semantics[i]];
      vertex_attrib attrib(
        i, array->element_size(), array->gltype(), vertex_size, offset, buf);
      offset += array->element_bytes();
      _vao->attrib_pointer(attrib);
    }

    assert(p - data.get() == total_bytes);

    buf->buffer_data(total_bytes, data.get(), GL_STATIC_DRAW);
    buf->set_data(std::move(data));
  }
}

//--------------------------------------------------------------------
bool common_geometry::has(attrib_semantic type)
{
  return std::find(_attrib_semantics.begin(), _attrib_semantics.end(), type) !=
         _attrib_semantics.end();
}

//--------------------------------------------------------------------
array_uptr common_geometry::build_colors(const array& vertices)
{
  auto colors = std::make_unique<vec4_array>();
  std::generate_n(std::back_inserter(*colors), vertices.size(),
    []() -> vec4 { return vec4(linearRand(vec3(0), vec3(1)), 1); });
  return colors;
}

}
