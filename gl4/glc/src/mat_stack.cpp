#include "mat_stack.h"
#include "glm.h"

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
  m_stack.push(top());
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
void mat_stack::clear(const mat4& m)
{
  while(!m_stack.empty())
    m_stack.pop();
  m_stack.push(m);
}

//--------------------------------------------------------------------
void mat_stack::rotate(GLfloat r, const vec3& axis)
{
  top() = glm::rotate(top(), r, axis);
}

//--------------------------------------------------------------------
void mat_stack::translate(const vec3& v)
{
  top() = glm::translate(top(), v);
}

//--------------------------------------------------------------------
void mat_stack::scale(const vec3& s)
{
  top() = glm::scale(top(), s);
}

//--------------------------------------------------------------------
void mat_stack::load_identity()
{
  top() = mat4(1);
}

//--------------------------------------------------------------------
void mat_stack::load_mat4(const mat4& m)
{
  top() = m;
}

//--------------------------------------------------------------------
void mat_stack::multily(const mat4& m)
{
  top() = top() * m;
}

//--------------------------------------------------------------------
void mat_stack::preMultily(const mat4& m)
{
  top() = m * top();
}

//--------------------------------------------------------------------
GLuint mat_stack::size()
{
  return m_stack.size();
}


}
