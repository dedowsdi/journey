#include "super_shape_3d.h"
#include <memory>

namespace zxd
{

//--------------------------------------------------------------------
void super_shape_3d::build_vertex()
{
  vec3_array_ptr vertices(new vec3_array);
  attrib_array(0, vertices); 
  vertices->reserve(m_stack * (m_slice + 1) * 2);

  GLfloat lat_step = (m_latitude_end - m_latitude_start) / m_stack;
  GLfloat lon_step = (m_longitude_end - m_longitude_start) / m_slice;

  vec3_vector points;
  points.reserve((m_stack + 1) * (m_slice + 1));

  // generate spherical points latitude by latitude
  for (int i = 0; i <= m_stack; ++i) {
    GLfloat phi = m_latitude_start + lat_step * i;

    GLfloat cos_phi = cos(phi);
    GLfloat sin_phi = sin(phi);
    GLfloat r2 = supershape(phi);
    GLfloat z = m_radius * r2 * sin_phi;

    for (int j = 0; j <= m_slice; ++j) {
      GLfloat theta = m_longitude_start + lon_step*j;
      GLfloat r1 = supershape(theta);

      GLfloat x = m_radius * r1 * cos(theta) * r2 * cos_phi;
      GLfloat y = m_radius * r1 * sin(theta) * r2 * cos_phi;

      points.push_back(vec3(x, y, z));
    }
  }

  m_primitive_sets.clear();
  GLuint num_vertices_per_stack = (m_slice + 1) * 2;

  // generate triangle strip latitude by latitude
  for (int i = 0; i < m_stack; ++i) {
    int stack_start0 = i * (m_slice + 1);
    int stack_start1 = stack_start0 + m_slice + 1;

    for (int j = 0; j <= m_slice; ++j) {
      vertices->push_back(points[stack_start1 + j]);
      vertices->push_back(points[stack_start0 + j]);
    }

    m_primitive_sets.push_back( std::make_shared<draw_arrays>(
          GL_TRIANGLE_STRIP, num_vertices_per_stack*i, num_vertices_per_stack));
  }

  std::cout << "create super shape 3d in " << vertices->size() << " vertices" << std::endl;
}

//--------------------------------------------------------------------
GLfloat super_shape_3d::supershape(GLfloat theta)
{
  if(m_a == 0 || m_b == 0)
    throw std::runtime_error("a or b can not be 0 in supershape3d");

  GLfloat cf0 = m_m * theta / 4;
  GLfloat p0 = pow(abs(1/m_a * cos(cf0)), m_n2);
  GLfloat p1 = pow(abs(1/m_b * sin(cf0)), m_n3);
  return pow(p0 + p1, -1/m_n1);
}

}
