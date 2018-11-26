#ifndef GL_GLC_STATEUTIL_H
#define GL_GLC_STATEUTIL_H
#include "glad/glad.h"

template <GLenum type>
class capability_guard {
protected:
  GLboolean m_old;

public:
  capability_guard(GLboolean b) {
    m_old = glIsEnabled(type);
    if (b) {
      glEnable(type);
    } else {
      glDisable(type);
    }
  }

  ~capability_guard() {
    if (m_old) {
      glEnable(type);
    } else {
      glDisable(type);
    }
  }
};

#endif /* GL_GLC_STATEUTIL_H */
