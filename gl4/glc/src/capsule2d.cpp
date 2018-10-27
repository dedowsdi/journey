#include "capsule2d.h"

namespace zxd
{

//--------------------------------------------------------------------
void capsule2d::build_vertex()
{
  vec2_array* vertices = new vec2_array;
  attrib_array(0, array_ptr(vertices));
  vertices->reserve(m_slice + 4);

  GLfloat half_rect_width = m_width * 0.5 - m_radius * 2;
  if(half_rect_width < 0)
    throw std::runtime_error("negative rect width");

  vertices->push_back(vec2(0, 0));
  GLfloat step_angle = f2pi / m_slice;
  vec2 center = vec2(half_rect_width, 0);

  for (int i = 0; i <= m_slice; ++i) 
  {
    GLfloat angle = -fpi2 + step_angle  * i; // from -fpi2 to fpi2 * 3
    vertices->push_back(center + m_radius * vec2(cos(angle), sin(angle)));

    // reflect center in half way
    if(i == m_slice/2)
    {
      center *= -1;
      vertices->push_back(center + m_radius * vec2(cos(angle), sin(angle)));
    }
  }
  vertices->push_back(vertices->at(1));

  assert(vertices->size() == m_slice + 4);

  add_primitive_set(new draw_arrays(GL_TRIANGLE_FAN, 0, vertices->size()));
}

}
