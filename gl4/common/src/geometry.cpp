#include "geometry.h"

namespace zxd {

//--------------------------------------------------------------------
void Geometry::bindVertexArrayObject() {
  if (mVao == -1) {
    glGenVertexArrays(1, &mVao);
  }
  glBindVertexArray(mVao);
}
}
