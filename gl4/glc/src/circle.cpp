#include "circle.h"

#include <glm/common.hpp>

namespace zxd
{

//--------------------------------------------------------------------
void circle::build_vertex()
{
  auto vertices = make_array<vec2_array>(0);

  if(m_type == FILL || m_type == PIE_LINE)
    vertices->push_back(vec2(0));

  GLfloat stepAngle = (m_end - m_start)/m_slice;
  for (int i = 0; i <= m_slice; ++i) 
  {
    GLfloat theta = m_start + i * stepAngle;
    GLfloat x = m_radius * cos(theta);
    GLfloat y = m_radius * sin(theta);
    vertices->push_back(vec2(x, y));
  }

  if(m_type == PIE_LINE)
    vertices->push_back(vec2(0));

  GLenum mode = m_type == FILL ? GL_TRIANGLE_FAN : GL_LINE_STRIP;
  add_primitive_set(new draw_arrays(mode, 0, vertices->size()));
}

}
