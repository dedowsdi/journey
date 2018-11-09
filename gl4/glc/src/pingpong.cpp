#include "pingpong.h"

//--------------------------------------------------------------------
pingpong::pingpong(GLuint r0, GLuint r1)
{
  ping(r0);
  pong(r1);
}

//--------------------------------------------------------------------
void pingpong::set(GLuint r0, GLuint r1)
{
  ping(r0);
  pong(r1);
}

//--------------------------------------------------------------------
void pingpong::swap()
{
  std::swap(m_ping, m_pong);
}

//--------------------------------------------------------------------
GLuint npingpong::pong()
{
  if(m_resources.empty())
    throw std::runtime_error("empty resources");
  return m_resources.back();
}

//--------------------------------------------------------------------
GLuint npingpong::last_ping()
{
  if(m_resources.size() <= 1)
    throw std::runtime_error("not enough resources");
  return m_resources[m_resources.size() - 2];
}

//--------------------------------------------------------------------
GLuint npingpong::first_ping()
{
  if(m_resources.size() <= 1)
    throw std::runtime_error("not enough resources");
  return m_resources.front();
}

//--------------------------------------------------------------------
GLuint npingpong::get(GLuint index)
{
  if(m_resources.size() <= index)
    throw std::runtime_error("resource index overflow");
  return m_resources[index];
}

//--------------------------------------------------------------------
GLuint npingpong::size()
{
  return m_resources.size();
}

//--------------------------------------------------------------------
void npingpong::add_resource(GLuint resource)
{
  m_resources.push_back(resource);
}

//--------------------------------------------------------------------
void npingpong::shift()
{
  auto temp = m_resources;
  m_resources.clear();
  m_resources.push_back(temp.back());
  m_resources.insert(m_resources.end(), temp.begin(), temp.end() - 1);
}
