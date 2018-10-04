#include "matstack.h"

namespace zxd
{

//--------------------------------------------------------------------
MatStack::MatStack()
{
  m_stack.push(osg::Matrix::identity());
}

//--------------------------------------------------------------------
void MatStack::push()
{
  m_stack.push(m_stack.top());
}

//--------------------------------------------------------------------
void MatStack::push_identify()
{
  m_stack.push(osg::Matrix::identity());
}

//--------------------------------------------------------------------
void MatStack::pop()
{
  if(m_stack.size() == 1)
    throw std::runtime_error("last entry in mat stack can not be popped");

  m_stack.pop();
}

//--------------------------------------------------------------------
const osg::Matrix& MatStack::top() const
{
  return m_stack.top();
}

//--------------------------------------------------------------------
osg::Matrix& MatStack::top()
{
  return m_stack.top();
}

//--------------------------------------------------------------------
void MatStack::rotate(float r, const osg::Vec3& axis)
{
  m_stack.top().preMult(osg::Matrix::rotate(r, axis));
}

//--------------------------------------------------------------------
void MatStack::translate(const osg::Vec3& v)
{
  m_stack.top().preMultTranslate(v);
}

//--------------------------------------------------------------------
void MatStack::scale(const osg::Vec3& s)
{
  m_stack.top().preMultScale(s);
}

//--------------------------------------------------------------------
void MatStack::load_identity()
{
  m_stack.top().makeIdentity();
}

//--------------------------------------------------------------------
void MatStack::load_mat4(const osg::Matrix& m)
{
  m_stack.top() = m;
}

//--------------------------------------------------------------------
void MatStack::postMultily(const osg::Matrix& m)
{
  m_stack.top() = m_stack.top() * m;
}

//--------------------------------------------------------------------
void MatStack::preMultily(const osg::Matrix& m)
{
  m_stack.top() = m * m_stack.top();
}

//--------------------------------------------------------------------
unsigned MatStack::size()
{
  return m_stack.size();
}

}
