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
