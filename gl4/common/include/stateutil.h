#ifndef STATEUTIL_H
#define STATEUTIL_H
#include "glad/glad.h"

template <GLenum type>
class CapabilityGuard {
protected:
  GLboolean mOld;

public:
  CapabilityGuard(GLboolean b) {
    mOld = glIsEnabled(type);
    if (b) {
      glEnable(type);
    } else {
      glDisable(type);
    }
  }

  ~CapabilityGuard() {
    if (mOld) {
      glEnable(type);
    } else {
      glDisable(type);
    }
  }
};

#endif /* STATEUTIL_H */
