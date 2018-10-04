#include "points.h"

namespace zxd {

//--------------------------------------------------------------------
GLint origin2::build_vertex()
{
  vec2_array& vertices = *(new vec2_array());
  attrib_array(num_arrays(), array_ptr(&vertices));
  vertices.push_back(vec2(0));

  return num_arrays() - 1;
}

//--------------------------------------------------------------------
void origin2::draw(GLuint primcount)
{
  bind_vao();
  draw_arrays(GL_POINTS, 0, 1, primcount);
}
}
