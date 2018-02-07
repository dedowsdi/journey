#include "cone.h"
#include "common.h"

namespace zxd {

//--------------------------------------------------------------------
void cone::build_vertex() {
  GLfloat theta_step = f2pi / m_slice;  // azimuthal angle step
  GLfloat height_step = m_height / m_stack;
  GLfloat radius_step = m_radius / m_stack;

  GLuint num_vert_pole = m_slice + 1;
  GLuint num_vert_center = (m_slice + 1) * 2;

  m_vertices.reserve(num_vert_pole * 2 + num_vert_center * (m_stack - 1));

  // add bottom fan
  m_vertices.push_back(vec3(0.0));
  for (int i = 0; i <= m_slice; ++i) {
    GLfloat theta = -theta_step * i;  // apply - for back face
    m_vertices.push_back(
      vec3(m_radius * glm::cos(theta), m_radius * glm::sin(theta), 0));
  }

  // create stack from bottom to top, different sphere.
  // build triangle strip as
  //    0 2
  //    1 3
  for (int i = 0; i < m_stack; ++i) {
    GLfloat h0 = height_step * i;
    GLfloat h1 = h0 + height_step;
    GLfloat r0 = radius_step * (m_stack - i);
    GLfloat r1 = r0 - radius_step;

    // add hat
    if (i == m_stack - 1) m_vertices.push_back(glm::vec3(0, 0, m_height));

    for (int j = 0; j <= m_slice; j++) {
      // loop last stack in reverse order
      GLfloat theta = theta_step * j;
      GLfloat cos_theta = std::cos(theta);
      GLfloat sin_theta = std::sin(theta);

      if (i != m_stack - 1) {
        m_vertices.push_back(vec3(r1 * cos_theta, r1 * sin_theta, h1));
      }

      m_vertices.push_back(vec3(r0 * cos_theta, r0 * sin_theta, h0));
    }
  }
}

//--------------------------------------------------------------------
void cone::build_normal() {
  m_normals.clear();
  m_normals.reserve(m_vertices.size());

  GLuint num_vert_bottom = m_slice + 2;

  // normals for bottom
  for (int i = 0; i < num_vert_bottom; ++i) {
    m_normals.push_back(vec3(0, 0, -1));
  }

  glm::vec3 apex(0, 0, m_height);

  // normals for 1st stack
  for (int i = 0; i <= m_slice; ++i) {
    const vec3& vertex = m_vertices[num_vert_bottom + i * 2];
    vec3 outer = glm::cross(vertex, apex);
    vec3 normal = glm::cross(apex - vertex, outer);
    m_normals.push_back(normal);
    m_normals.push_back(normal);
  }

  // normals for 1 - last 2 stacks
  for (int i = 1; i < m_stack - 1; ++i) {
    // no reallocate will happen here
    m_normals.insert(m_normals.end(), m_normals.begin() + num_vert_bottom,
      m_normals.begin() + num_vert_bottom + (m_slice + 1) * 2);
  }

  // normals for last stacks
  m_normals.push_back(vec3(0, 0, 1));
  for (int i = 0; i <= m_slice; ++i) {
    m_normals.push_back(m_normals[num_vert_bottom + i * 2]);
  }
  assert(m_normals.size() == m_vertices.size());
}

//--------------------------------------------------------------------
void cone::build_texcoord() {
  m_texcoords.clear();
  m_texcoords.reserve(m_vertices.size());

  // bottom texcoords
  m_texcoords.push_back(glm::vec2(0, 0));
  for (int i = 0; i <= m_slice; ++i) {
    GLfloat s = 1 - static_cast<GLfloat>(i) / m_slice;
    m_texcoords.push_back(glm::vec2(s, 0));
  }

  for (int i = 0; i < m_stack; ++i) {
    GLfloat t0 = static_cast<GLfloat>(i + 1) / m_stack;
    GLfloat t1 = static_cast<GLfloat>(i) / m_stack;

    // add pole
    if (i == m_stack - 1) m_texcoords.push_back(glm::vec2(1, 0));

    for (int j = 0; j <= m_slice; j++) {
      // loop last stack in reverse order
      GLfloat s = static_cast<GLfloat>(j) / m_slice;

      m_texcoords.push_back(glm::vec2(s, t0));

      if (i != m_stack - 1) m_texcoords.push_back(glm::vec2(s, t1));
    }
  }

  assert(m_texcoords.size() == m_vertices.size());
}

//--------------------------------------------------------------------
void cone::draw(GLuint primcount /* = 1*/) {
  GLuint num_vert_pole = m_slice + 2;  // pole + slice + 1
  GLuint num_vert_center = (m_slice + 1) * 2;

  bind_vertex_array_object();

  draw_arrays(GL_TRIANGLE_FAN, 0, num_vert_pole, primcount);

  GLuint next = num_vert_pole;
  for (int i = 0; i < m_stack - 1; ++i, next += num_vert_center) {
    draw_arrays(GL_TRIANGLE_STRIP, next, num_vert_center, primcount);
  }
  draw_arrays(GL_TRIANGLE_FAN, next, num_vert_pole, primcount);
}
}
