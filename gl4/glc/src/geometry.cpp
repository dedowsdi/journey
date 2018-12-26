#include "geometry.h"
#include "string_util.h"

namespace zxd {

//--------------------------------------------------------------------
void geometry_base::attrib_array(GLuint index, array_ptr _array) {
  if (index >= m_attributes.size()) {
    m_attributes.resize(index + 1);
  }
  m_attributes[index] = _array;
}

//--------------------------------------------------------------------
array_ptr geometry_base::attrib_array(GLuint index)  const{
  if (index >= m_attributes.size()) {
    std::cerr << " attribute index overflow : " << index << std::endl;
  }
  return m_attributes[index];
}

//--------------------------------------------------------------------
float_array_ptr geometry_base::attrib_float_array(GLuint index) const {
  return std::static_pointer_cast<float_array>(attrib_array(index));
}

//--------------------------------------------------------------------
vec2_array_ptr geometry_base::attrib_vec2_array(GLuint index)  const{
  return std::static_pointer_cast<vec2_array>(attrib_array(index));
}

//--------------------------------------------------------------------
vec3_array_ptr geometry_base::attrib_vec3_array(GLuint index)  const{
  return std::static_pointer_cast<vec3_array>(attrib_array(index));
}

//--------------------------------------------------------------------
vec4_array_ptr geometry_base::attrib_vec4_array(GLuint index)  const{
  return std::static_pointer_cast<vec4_array>(attrib_array(index));
}

//--------------------------------------------------------------------
GLuint geometry_base::num_vertices() { return attrib_array(0)->num_elements(); }

//--------------------------------------------------------------------
void geometry_base::draw()
{
  bind_vao();
  for (int i = 0; i < m_primitive_sets.size(); ++i) {
    m_primitive_sets[i]->draw();
  }
}

//--------------------------------------------------------------------
void geometry_base::build_mesh() {
  m_primitive_sets.clear();
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
    if(m_attributes[i]->num_elements() == 0)
    {
      std::cout << "found empty array attribute : " << i << std::endl;
      continue;
    }

    m_attributes[i]->bind(i);
    m_attributes[i]->update_buffer();
  }

  if(m_elements)
    m_elements->update_buffer();
}

//--------------------------------------------------------------------
void geometry_base::bind_and_update_buffer(GLint index)
{
  bind_vao();
  if(index >= m_attributes.size())
    throw std::runtime_error("illegal attrib index " + string_util::to(index));
}

//--------------------------------------------------------------------
primitive_set* geometry_base::get_primitive_set(GLuint index)
{
  if(index >= m_primitive_sets.size())
    throw std::runtime_error("primitive set index over flow");
  return m_primitive_sets[index].get();
}

//--------------------------------------------------------------------
GLuint geometry_base::get_num_primitive_set()
{
  return m_primitive_sets.size();
}

//--------------------------------------------------------------------
void geometry_base::add_primitive_set(primitive_set* ps)
{
  // this might cause problem, i should use shared_ptr as argument!!!
  m_primitive_sets.push_back(std::shared_ptr<primitive_set>(ps));
}

//--------------------------------------------------------------------
void geometry_base::clear_primitive_sets()
{
  m_primitive_sets.clear();
}

//--------------------------------------------------------------------
void geometry_base::remove_primitive_sets(GLuint index, GLuint count)
{
  if(index + count > m_primitive_sets.size())
    throw std::runtime_error("error, too many remove count");
  m_primitive_sets.erase(m_primitive_sets.begin() + index, m_primitive_sets.begin() + index + count);
}

//--------------------------------------------------------------------
void geometry_base::set_num_instance(GLuint count)
{
  for (int i = 0; i < m_primitive_sets.size(); ++i) {
    m_primitive_sets[i]->num_instance(count);
  }
}

//--------------------------------------------------------------------
void geometry_base::accept(primitive_functor& pf) const
{
  vec3_array* vertices = attrib_vec3_array(0).get();
  pf.set_vertex_array(vertices->size(), &vertices->front());
  for(const auto& item : m_primitive_sets)
  {
    item->accept(pf);
  }
  
}

//--------------------------------------------------------------------
void geometry_base::bind_vao() {
  if (m_vao == -1) {
    glGenVertexArrays(1, &m_vao);
  }
  glBindVertexArray(m_vao);
}

//--------------------------------------------------------------------
void geometry_base::clear()
{
  m_primitive_sets.clear();
  m_attributes.clear();
  m_elements.reset();
}
}
