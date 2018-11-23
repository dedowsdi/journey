#include "lissajous.h"

namespace zxd
{

//--------------------------------------------------------------------
void lissajous::build_vertex()
{
  auto vertices = std::make_shared<vec2_array>();
  attrib_array(0, vertices);

  vertices->reserve(m_slices + 1);

  GLfloat step_angle = period() / m_slices;
  for (int i = 0; i <= m_slices; ++i) {
    vertices->push_back(get_at_angle(step_angle * i));
  }

  add_primitive_set(new draw_arrays(GL_LINE_STRIP, 0, vertices->size()));
}


//--------------------------------------------------------------------
GLfloat lissajous::period()
{
  if(m_type == LT_CIRCLE)
    return f2pi * zxd::lcm(m_xscale, m_yscale) / (m_xscale * m_yscale);
  else 
  {
    GLuint g = zxd::gcd(m_rose_n, m_rose_d);
    GLuint n = m_rose_n / g;
    GLuint d = m_rose_d / g;
    return d&1 && n&1 && m_rose_offset == 0 ? (d * fpi) : (d * f2pi);
  }
}

//--------------------------------------------------------------------
vec2 lissajous::get_at_angle(GLfloat angle)
{
  GLfloat xangle = m_xscale * angle;
  GLfloat yangle = m_yscale * angle;
  if(m_type == LT_CIRCLE)
    return vec2(cos(xangle), sin(yangle)) * m_radius;
  else
  {
    GLfloat scale = 1;
    if(m_rose_offset != 0)
      scale = m_rose_offset > 0 ? 1.0 / (1 + m_rose_offset) : 1.0 / (1 - m_rose_offset);
    GLfloat k = static_cast<GLfloat>(m_rose_n) / m_rose_d;
    GLfloat rx = cos(k * xangle) + m_rose_offset;
    GLfloat x = rx * cos(xangle) * m_radius;
    GLfloat ry = cos(k * yangle) + m_rose_offset;
    GLfloat y = ry * sin(yangle) * m_radius;
    return vec2(x, y) * scale;
  }
}

}
