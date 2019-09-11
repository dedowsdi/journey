#include <super_shape_2d.h>

#include <glm/trigonometric.hpp>
#include <glm/common.hpp>
#include <glm/exponential.hpp>
#include <glmath.h>

namespace zxd
{


//--------------------------------------------------------------------
common_geometry::vertex_build super_shape_2d::build_vertices()
{
  if(m_a == 0 || m_b == 0)
    throw std::runtime_error("a or b can not be 0 in supershape2d");

  auto vertices = std::make_unique<vec2_array>();
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

  clear_primitive_sets();
  add_primitive_set(
    std::make_unique<draw_arrays>(GL_TRIANGLE_FAN, 0, vertices->size()));
  return vertex_build{std::move(vertices)};
}

}
