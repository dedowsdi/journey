#include <axes.h>

namespace zxd
{

//--------------------------------------------------------------------
common_geometry::vertex_build axes::build_vertices()
{
  auto vertices = std::make_unique<vec3_array>();
  vertices->reserve(6);

  vertices->push_back(vec3(0, 0, 0));
  vertices->push_back(vec3(1, 0, 0));
  vertices->push_back(vec3(0, 0, 0));
  vertices->push_back(vec3(0, 1, 0));
  vertices->push_back(vec3(0, 0, 0));
  vertices->push_back(vec3(0, 0, 1));

  clear_primitive_sets();
  add_primitive_set(std::make_shared<draw_arrays>(GL_LINES, 0, 6));
  return vertex_build{std::move(vertices)};
}

//--------------------------------------------------------------------
array_uptr axes::build_colors(const array& vertices)
{
  auto colors = std::make_unique<vec3_array>();
  colors->reserve(6);

  colors->push_back(vec3(1, 0, 0));
  colors->push_back(vec3(1, 0, 0));
  colors->push_back(vec3(0, 1, 0));
  colors->push_back(vec3(0, 1, 0));
  colors->push_back(vec3(0, 0, 1));
  colors->push_back(vec3(0, 0, 1));

  return colors;
}

}
