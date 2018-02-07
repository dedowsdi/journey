#include "disk.h"

namespace zxd {

//--------------------------------------------------------------------
void disk::build_vertex() {
  m_vertices.clear();
  GLuint cv_ring = (m_slice + 1) * 2;
  GLuint cv_disk =
    m_inner == 0 ? cv_ring * (m_loop - 1) + m_slice + 2 : cv_ring * m_loop;
  m_vertices.reserve(cv_disk);

  GLfloat theta_step = f2pi / m_slice;
  GLfloat radius_step = (m_outer - m_inner) / m_loop;
  GLuint li = 0;

  // if inner is 0, build innder circle with tirangle fan
  if (m_inner == 0) {
    m_vertices.push_back(vec3(0));
    GLfloat radius = radius_step;

    for (int i = 0; i <= m_slice; ++i) {
      GLfloat theta = theta_step * i;
      GLfloat ct = std::cos(theta);
      GLfloat st = std::sin(theta);

      m_vertices.push_back(vec3(radius * ct, radius * st, 0));
    }

    ++li;
  }

  // build out rings with triangle strip
  for (; li < m_loop; ++li) {
    GLfloat radius0 = m_inner + radius_step * li;
    GLfloat radius1 = m_inner + radius_step * (li + 1);

    for (int i = 0; i <= m_slice; ++i) {
      GLfloat theta = theta_step * i;
      GLfloat ct = std::cos(theta);
      GLfloat st = std::sin(theta);

      m_vertices.push_back(vec3(radius0 * ct, radius0 * st, 0));
      m_vertices.push_back(vec3(radius1 * ct, radius1 * st, 0));
    }
  }
}

//--------------------------------------------------------------------
void disk::build_normal() {
  m_normals.clear();
  m_normals.reserve(m_vertices.size());
  for (int i = 0; i < m_vertices.size(); ++i) {
    m_normals.push_back(vec3(0, 0, 1));
  }
}

//--------------------------------------------------------------------
void disk::build_texcoord() {
  m_texcoords.clear();
  m_texcoords.reserve(m_vertices.size());

  for (int i = 0; i < m_vertices.size(); ++i) {
    const vec3& vertex = m_vertices[i];
    m_texcoords.push_back(vertex.xy() * 0.5f / m_outer + 0.5f);
  }
}

//--------------------------------------------------------------------
void disk::draw(GLuint primcount /* = 1*/) {
  GLuint cv_ring = (m_slice + 1) * 2;
  GLint cv_fan = m_slice + 2;
  GLuint li = 0;
  GLuint next = 0;

  bind_vertex_array_object();

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
