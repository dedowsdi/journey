#include "mat_stack.h"

namespace zxd
{

//--------------------------------------------------------------------
mat_stack::mat_stack()
{
  m_stack.push(mat4(1));
}

//--------------------------------------------------------------------
void mat_stack::push()
{
  m_stack.push(m_stack.top());
}

//--------------------------------------------------------------------
void mat_stack::push_identify()
{
  m_stack.push(mat4(1));
}

//--------------------------------------------------------------------
void mat_stack::pop()
{
  if(m_stack.size() == 1)
    throw std::runtime_error("last entry in mat stack can not be popped");

  m_stack.pop();
}

//--------------------------------------------------------------------
const mat4& mat_stack::top() const
{
  return m_stack.top();
}

//--------------------------------------------------------------------
mat4& mat_stack::top()
{
  return m_stack.top();
}

//--------------------------------------------------------------------
void mat_stack::rotate(GLfloat r, const vec3& axis)
{
  m_stack.top() = glm::rotate(m_stack.top(), r, axis);
}

//--------------------------------------------------------------------
void mat_stack::translate(const vec3& v)
{
  m_stack.top() = glm::translate(m_stack.top(), v);
}

//--------------------------------------------------------------------
void mat_stack::scale(const vec3& s)
{
  m_stack.top() = glm::scale(m_stack.top(), s);
}

//--------------------------------------------------------------------
void mat_stack::load_identity()
{
  m_stack.top() = mat4(1);
}

//--------------------------------------------------------------------
void mat_stack::load_mat4(const mat4& m)
{
  m_stack.top() = m;
}

//--------------------------------------------------------------------
void mat_stack::multily(const mat4& m)
{
  m_stack.top() = (m_stack.top() * m);
}

//--------------------------------------------------------------------
void mat_stack::preMultily(const mat4& m)
{
  m_stack.top() = (m * m_stack.top());
}

//--------------------------------------------------------------------
GLuint mat_stack::size()
{
  return m_stack.size();
}


}
