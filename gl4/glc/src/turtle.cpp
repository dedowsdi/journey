#include <turtle.h>

#include <stdexcept>

#include "glm/glm.hpp"
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <vao.h>

namespace  zxd
{

//--------------------------------------------------------------------
translate_operation::translate_operation(const vec3& translation):
  m_translation(translation)
{
}

//--------------------------------------------------------------------
void translate_operation::operator()(turtle* t) const
{
  t->translate(m_translation);
}

//--------------------------------------------------------------------
translate_only_operation::translate_only_operation(const vec3& translation):
  m_translation(translation)
{
}

//--------------------------------------------------------------------
void translate_only_operation::operator()(turtle* t) const
{
  t->translate_only(m_translation);
}

//--------------------------------------------------------------------
rotate_operation::rotate_operation(GLfloat angle, const vec3& axis):
  m_angle(angle),
  m_axis(axis)
{

}

//--------------------------------------------------------------------
void rotate_operation::operator()(turtle* t) const
{
  t->rotate(m_angle, m_axis);
}

//--------------------------------------------------------------------
void push_operation::operator()(turtle* t) const
{
  t->push();
}

//--------------------------------------------------------------------
void pop_operation::operator()(turtle* t) const
{
  t->pop();
}

//--------------------------------------------------------------------
std::vector<vec3> turtle::generate(const std::string& pattern, const mat4& start_transform)
{
  m_transform.clear(start_transform);
  m_vertices.clear();

  //vec3 p = column(m_transform.top(), 3).xyz();
  //m_vertices.push_back(p);

  for (auto c : pattern) 
  {
    auto operation = get_operation(c);
    if(!operation)
      continue;

    (*operation)(this);
  }

  return std::move(m_vertices);
}

//--------------------------------------------------------------------
void turtle::translate(const vec3& v)
{
  vec3 p0 = vec3(column(m_transform.top(), 3));
  m_transform.translate(v);
  vec3 p1 = vec3(column(m_transform.top(), 3));
  m_vertices.push_back(p0);
  m_vertices.push_back(p1);
}

//--------------------------------------------------------------------
void turtle::translate_only(const vec3& v)
{
  m_transform.translate(v);
}

//--------------------------------------------------------------------
void turtle::rotate(GLfloat angle, const vec3& axis)
{
  m_transform.rotate(angle, axis);
}

//--------------------------------------------------------------------
void turtle::load_transform(const mat4& m)
{
  m_transform.load_mat4(m);
}

//--------------------------------------------------------------------
const mat4& turtle::get_transform() const
{
  return m_transform.top();
}

//--------------------------------------------------------------------
void turtle::push()
{
  m_transform.push();
}

//--------------------------------------------------------------------
void turtle::pop()
{
  m_transform.pop();
}

//--------------------------------------------------------------------
void turtle::register_operation(char c, const spto& operation)
{
  m_operations[c] = operation;
}

//--------------------------------------------------------------------
const turtle_operation* turtle::get_operation(char c) const
{
  auto iter = m_operations.find(c);
  return iter == m_operations.end() ? nullptr : iter->second.get();
}
}
