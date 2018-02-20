#include "points.h"

namespace zxd {

//--------------------------------------------------------------------
void Points3::draw(GLuint primcount) {
  bind_vao();
  draw_arrays(GL_POINTS, 0, num_vertices(), primcount);
}
}
