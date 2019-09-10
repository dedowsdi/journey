#include "rose.h"

#include "glmath.h"
#include <glm/common.hpp>

namespace zxd
{

//--------------------------------------------------------------------
rose::rose(GLuint n, GLuint d):
  m_n(n),
  m_d(d)
{
}

//--------------------------------------------------------------------
common_geometry::vertex_build rose::build_vertices()
{
  auto vertices = std::make_unique<vec2_array>();

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

  GLfloat scale = 1;
  if(m_offset != 0)
    scale = m_offset > 0 ? 1.0 / (1 + m_offset) : 1.0 / (1 - m_offset);

  for (int i = 0; i <= m_slice; ++i) 
  {
    GLfloat theta = start_angle + step_angle * i;
    GLfloat ktheta = k*theta;
    GLfloat r = cos(ktheta) + m_offset;
    GLfloat x = r * cos(theta) * m_radius;
    GLfloat y = r * sin(theta) * m_radius;
    vertices->push_back(vec2(x,y) * scale);
  }

  add_primitive_set(std::make_shared<draw_arrays>(GL_LINE_STRIP, 0, vertices->size()));
  return vertex_build{std::move(vertices)};
}

//--------------------------------------------------------------------
vec2 rose::get_rose(
  GLfloat radius, GLfloat angle, GLuint n_, GLuint d_, GLfloat offset)
{
  GLuint g = zxd::gcd(n_, d_);
  GLuint n = n_ / g;
  GLuint d = d_ / g;
  GLfloat k = static_cast<GLfloat>(n) / d;

  GLfloat scale = offset == 0 ? 1 : 1.0 / (1 + abs(offset));

  GLfloat ktheta = k*angle;
  GLfloat r = (cos(ktheta) + offset) * radius * scale;
  GLfloat x = r * cos(angle);
  GLfloat y = r * sin(angle);

  return vec2(x, y);
}

}
