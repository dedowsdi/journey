#include "axes.h"

namespace zxd {

//--------------------------------------------------------------------
void axes::build_vertex() {
  vec3_array& vertices = *(new vec3_array());
  attrib_array(num_arrays(), array_ptr(&vertices));
  vertices.reserve(6);

  vertices.push_back(vec3(0, 0, 0));
  vertices.push_back(vec3(1, 0, 0));
  vertices.push_back(vec3(0, 0, 0));
  vertices.push_back(vec3(0, 1, 0));
  vertices.push_back(vec3(0, 0, 0));
  vertices.push_back(vec3(0, 0, 1));

}

//--------------------------------------------------------------------
void axes::build_color() {
  vec3_array& colors = *(new vec3_array());
  attrib_array(num_arrays(), array_ptr(&colors));
  colors.reserve(6);

  colors.push_back(vec3(1, 0, 0));
  colors.push_back(vec3(1, 0, 0));
  colors.push_back(vec3(0, 1, 0));
  colors.push_back(vec3(0, 1, 0));
  colors.push_back(vec3(0, 0, 1));
  colors.push_back(vec3(0, 0, 1));

}

//--------------------------------------------------------------------
void axes::draw(GLuint primcount /* = 1*/) {
  draw_arrays(GL_LINES, 0, 6, primcount);
}
}
