#include "super_shape_2d.h"

namespace zxd
{


//--------------------------------------------------------------------
void super_shape_2d::build_vertex()
{
  if(m_a == 0 || m_b == 0)
    throw std::runtime_error("a or b can not be 0 in supershape2d");

  vec2_array* vertices = new vec2_array;
  attrib_array(0, array_ptr(vertices));
  vertices->reserve(m_num_slice + 2);

  vertices->push_back(vec2(0));
  GLfloat step_angle = f2pi / m_num_slice;
  for (int i = 0; i <= m_num_slice; ++i) 
  {
    GLfloat angle = step_angle * i;
    GLfloat cf0 = m_m / 4 * angle;
    
    GLfloat lhs = glm::pow(glm::abs(1/m_a * glm::cos(cf0)), m_n2);
    GLfloat rhs = glm::pow(glm::abs(1/m_b * glm::sin(cf0)), m_n3);
    GLfloat r = m_radius / glm::pow(lhs + rhs, 1/m_n1);

    vertices->push_back(glm::vec2(r * glm::cos(angle), r * glm::sin(angle)));
  }

  m_primitive_sets.clear();
  add_primitive_set(new draw_arrays(GL_TRIANGLE_FAN, 0, attrib_array(0)->num_elements()));
}

}
