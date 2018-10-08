#include "points.h"

namespace zxd {

//--------------------------------------------------------------------
void origin2::build_vertex()
{
  vec2_array& vertices = *(new vec2_array());
  attrib_array(num_arrays(), array_ptr(&vertices));
  vertices.push_back(vec2(0));

  m_primitive_sets.clear();
  add_primitive_set(new draw_arrays(GL_POINTS, 0, 1));
}

}
