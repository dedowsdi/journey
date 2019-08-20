#include "axes.h"

namespace zxd
{

//--------------------------------------------------------------------
void axes::build_vertex()
{
  auto vertices = make_array<vec3_array>(0);
  vertices->reserve(6);

  vertices->push_back(vec3(0, 0, 0));
  vertices->push_back(vec3(1, 0, 0));
  vertices->push_back(vec3(0, 0, 0));
  vertices->push_back(vec3(0, 1, 0));
  vertices->push_back(vec3(0, 0, 0));
  vertices->push_back(vec3(0, 0, 1));

  m_primitive_sets.clear();
  add_primitive_set(new draw_arrays(GL_LINES, 0, 6));
}

//--------------------------------------------------------------------
void axes::build_color()
{
  auto colors = make_array<vec3_array>(num_arrays());
  colors->reserve(6);

  colors->push_back(vec3(1, 0, 0));
  colors->push_back(vec3(1, 0, 0));
  colors->push_back(vec3(0, 1, 0));
  colors->push_back(vec3(0, 1, 0));
  colors->push_back(vec3(0, 0, 1));
  colors->push_back(vec3(0, 0, 1));

}

}
