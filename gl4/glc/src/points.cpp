#include "points.h"

namespace zxd {

//--------------------------------------------------------------------
void Points3::draw(GLuint primcount) {
  bind_vertex_array_object();
  draw_arrays(GL_POINTS, 0, m_vertices.size(), primcount);
}
}
