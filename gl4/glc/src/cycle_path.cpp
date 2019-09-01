#include "cycle_path.h"
#include <glm/glm.hpp>

namespace zxd
{

//--------------------------------------------------------------------
cycle_path::cycle_path(
  GLfloat a0, GLfloat b0, GLfloat c0, GLfloat a1, GLfloat b1, GLfloat c1)
    : m_a0(a0), m_b0(b0), m_c0(c0), m_a1(a1), m_b1(b1), m_c1(c1)
{
}

//--------------------------------------------------------------------
glm::vec2 cycle_path::get(GLfloat t)
{
  return vec2(sin(m_a0 * t + cos(m_b0 * t)) * cos(m_c0 * t),
    cos(m_a1 * t + sin(m_b1 * t)) * sin(m_c1 * t));
}

//--------------------------------------------------------------------
glm::vec2 cycle_path::tangent(GLfloat t)
{
  float at0 = m_a0 * t;
  float bt0 = m_b0 * t;
  float ct0 = m_c0 * t;
  float at1 = m_a1 * t;
  float bt1 = m_b1 * t;
  float ct1 = m_c1 * t;

  float cbt0 = cos(bt0);
  float sbt1 = sin(bt1);
  float x = sin(at0 + cbt0) * (-m_c0 * sin(ct0)) + cos(at0 + cbt0) * (m_a0 - m_b0 * sin(bt0)) * cos(ct0);
  float y = cos(at1 + sbt1) * (m_c1 * cos(ct1)) - sin(at1 + sbt1) * (m_a1 + m_b1 * cos(bt1)) * sin(ct1);
  return glm::normalize(vec2(x, y));
}
}
