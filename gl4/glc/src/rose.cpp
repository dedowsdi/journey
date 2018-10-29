#include "rose.h"

namespace zxd
{

//--------------------------------------------------------------------
void rose::build_vertex()
{
  vec2_array* vertices = new vec2_array();
  attrib_array(0, std::shared_ptr<vec2_array>(vertices));

  // simplify n and d
  GLuint g = zxd::gcd(m_n, m_d);
  GLuint n = m_n / g;
  GLuint d = m_d / g;

  GLfloat period = d&1 && n&1 && m_offset == 0 ? (d * fpi) : (d * f2pi);

  vertices->clear();
  vertices->reserve(m_slice + 1);

  GLfloat k = static_cast<GLfloat>(n) / d;

  GLfloat step_angle = period *(m_end - m_start) / m_slice;
  GLfloat start_angle = m_start * period;
  for (int i = 0; i <= m_slice; ++i) 
  {
    GLfloat theta = start_angle + step_angle * i;
    GLfloat ktheta = k*theta;
    GLfloat r = cos(ktheta) + m_offset;
    GLfloat x = r * cos(theta) * m_radius;
    GLfloat y = r * sin(theta) * m_radius;
    vertices->push_back(vec2(x,y));
  }

  add_primitive_set(new draw_arrays(GL_LINE_STRIP, 0, vertices->size()));
}

}
