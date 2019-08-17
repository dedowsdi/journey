#ifndef GL_GLC_STATEUTIL_H
#define GL_GLC_STATEUTIL_H
#include "glad/glad.h"

class cap_guard
{
public:
  cap_guard(GLenum cap, GLboolean b):
    m_cap(cap)
  {
    m_old = glIsEnabled(cap);
    if (b)
    {
      glEnable(cap);
    }
    else
    {
      glDisable(cap);
    }
  }

  ~cap_guard()
  {
    if (m_old)
    {
      glEnable(m_cap);
    }
    else
    {
      glDisable(m_cap);
    }
  }

private:
  GLboolean m_old;
  GLenum m_cap;

};

#endif /* GL_GLC_STATEUTIL_H */
