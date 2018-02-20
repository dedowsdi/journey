#include "disk.h"

namespace zxd {

//--------------------------------------------------------------------
GLint disk::build_vertex() {
  GLuint cv_ring = (m_slice + 1) * 2;
  GLuint cv_disk =
    m_inner == 0 ? cv_ring * (m_loop - 1) + m_slice + 2 : cv_ring * m_loop;
  vec3_array& vertices = *(new vec3_array());
  attrib_array(num_arrays(), array_ptr(&vertices));
  vertices.reserve(cv_disk);

  GLfloat theta_step = m_sweep / m_slice;
  GLfloat radius_step = (m_outer - m_inner) / m_loop;
  GLuint li = 0;

  // if inner is 0, build innder circle with tirangle fan
  if (m_inner == 0) {
    vertices.push_back(vec3(0));
    GLfloat radius = radius_step;

    for (int i = 0; i <= m_slice; ++i) {
      GLfloat theta = m_start + theta_step * i;
      GLfloat ct = std::cos(theta);
      GLfloat st = std::sin(theta);

      vertices.push_back(vec3(radius * ct, radius * st, 0));
    }

    ++li;
  }

  // build out rings with triangle strip
  for (; li < m_loop; ++li) {
    GLfloat radius0 = m_inner + radius_step * li;
    GLfloat radius1 = m_inner + radius_step * (li + 1);

    for (int i = 0; i <= m_slice; ++i) {
      GLfloat theta = m_start + theta_step * i;
      GLfloat ct = std::cos(theta);
      GLfloat st = std::sin(theta);

      vertices.push_back(vec3(radius0 * ct, radius0 * st, 0));
      vertices.push_back(vec3(radius1 * ct, radius1 * st, 0));
    }
  }
  return num_arrays() - 1;
}

//--------------------------------------------------------------------
GLint disk::build_normal() {
  vec3_array& normals = *(new vec3_array());
  attrib_array(num_arrays(), array_ptr(&normals));
  normals.reserve(num_vertices());
  for (int i = 0; i < num_vertices(); ++i) {
    normals.push_back(vec3(0, 0, 1));
  }
  return num_arrays() - 1;
}

//--------------------------------------------------------------------
GLint disk::build_texcoord() {
  vec2_array& texcoords = *(new vec2_array());
  attrib_array(num_arrays(), array_ptr(&texcoords));
  texcoords.reserve(num_vertices());
  const vec3_array& vertices = *attrib_vec3_array(0);

  for (int i = 0; i < num_vertices(); ++i) {
    const vec3& vertex = vertices[i];
    texcoords.push_back(vertex.xy() * 0.5f / m_outer + 0.5f);
  }
  return num_arrays() - 1;
}

//--------------------------------------------------------------------
void disk::draw(GLuint primcount /* = 1*/) {
  GLuint cv_ring = (m_slice + 1) * 2;
  GLint cv_fan = m_slice + 2;
  GLuint li = 0;
  GLuint next = 0;

  bind_vao();

  if (m_inner == 0) {
    draw_arrays(GL_TRIANGLE_FAN, 0, cv_fan, primcount);
    ++li;
    next += cv_fan;
  }

  for (; li < m_loop; ++li) {
    draw_arrays(GL_TRIANGLE_STRIP, next, cv_ring, primcount);
    next += cv_ring;
  }
}
}
