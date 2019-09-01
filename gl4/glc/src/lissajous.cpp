#include "lissajous.h"

#include "glmath.h"
#include <glm/common.hpp>
#include <glm/gtx/norm.hpp>

namespace zxd
{
//--------------------------------------------------------------------
void lissajous::build_vertex()
{
  auto vertices = std::make_shared<vec2_array>();
  attrib_array(0, vertices);

  vertices->reserve(m_slices + 1);

  GLfloat step_angle = period() / m_slices;
  for (int i = 0; i <= m_slices; ++i)
  {
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
glm::vec2 lissajous::get_at_angle(GLfloat angle) const
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

//--------------------------------------------------------------------
glm::vec2 lissajous::tangent_at_angle(GLfloat angle) const
{
  GLfloat xangle = m_xscale * angle;
  GLfloat yangle = m_yscale * angle;

  if(m_type == LT_CIRCLE)
    return glm::normalize(vec2(-m_xscale * -sin(angle), -m_yscale * cos(angle)));

  GLfloat k = static_cast<GLfloat>(m_rose_n) / m_rose_d;
  GLfloat rx = cos(k * angle) + m_rose_offset;
  GLfloat ry = cos(k * angle) + m_rose_offset;

  GLfloat dx_dt = -rx * sin(angle) - k * sin(k*angle) * cos(angle);
  GLfloat dy_dt = ry * cos(angle) - k * sin(k*angle) * sin(angle);

  return glm::normalize(vec2(dx_dt * m_xscale, dy_dt * m_yscale));
}

//--------------------------------------------------------------------
glm::vec2 lissajous::normal_at_angle(GLfloat angle) const
{
  glm::vec2 tangent = tangent_at_angle(angle);
  return vec2(tangent.y, -tangent.x);
}

}
