#include "cylinder.h"
#include "common.h"

namespace zxd {

//--------------------------------------------------------------------
void cylinder::build_vertex() {
  GLfloat theta_step =
    2 * glm::pi<GLfloat>() / m_slice;  // azimuthal angle step
  GLfloat height_step = m_height / m_stack;

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

  // create stack from bottom to top, different from sphere
  // build triangle strip as
  //    0 2
  //    1 3
  for (int i = 0; i < m_stack; ++i) {
    for (int j = 0; j <= m_slice; j++) {
      // loop last stack in reverse order
      GLfloat theta = theta_step * j;
      GLfloat cos_theta = std::cos(theta);
      GLfloat sin_theta = std::sin(theta);

      vec3 v1(m_radius * cos_theta, m_radius * sin_theta, height_step * i);
      vec3 v0 = v1;
      v0.z += height_step;

      m_vertices.push_back(v0);
      m_vertices.push_back(v1);
    }
  }

  // add top fan
  m_vertices.push_back(vec3(0, 0, m_height));
  for (int i = 0; i <= m_slice; ++i) {
    GLfloat theta = theta_step * i;  // apply - for back face
    m_vertices.push_back(
      vec3(m_radius * glm::cos(theta), m_radius * glm::sin(theta), m_height));
  }
}

//--------------------------------------------------------------------
void cylinder::build_normal() {
  m_normals.clear();
  m_normals.reserve(m_vertices.size());

  GLuint num_vert_bottom = m_slice + 2;

  // normals for bottom
  for (int i = 0; i < num_vert_bottom; ++i) {
    m_normals.push_back(vec3(0, 0, -1));
  }

  // normals for 1st stack
  for (int i = 0; i <= m_slice; ++i) {
    vec3 vertex = m_vertices[num_vert_bottom + i * 2];
    vertex.z = 0;
    vec3 normal = glm::normalize(vertex);
    m_normals.push_back(normal);
    m_normals.push_back(normal);
  }

  // normals for other stacks
  for (int i = 1; i < m_stack; ++i) {
    // no reallocate will happen here
    m_normals.insert(m_normals.end(), m_normals.begin() + num_vert_bottom,
      m_normals.begin() + num_vert_bottom + (m_slice + 1) * 2);
  }

  // normals for bottom
  for (int i = 0; i < num_vert_bottom; ++i) {
    m_normals.push_back(vec3(0, 0, 1));
  }
  assert(m_normals.size() == m_vertices.size());
}

//--------------------------------------------------------------------
void cylinder::build_texcoord() {
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

    for (int j = 0; j <= m_slice; j++) {
      // loop last stack in reverse order
      GLfloat s = static_cast<GLfloat>(j) / m_slice;

      m_texcoords.push_back(glm::vec2(s, t0));
      m_texcoords.push_back(glm::vec2(s, t1));
    }
  }

  // top tex coords
  m_texcoords.push_back(glm::vec2(0, 1));
  for (int i = 0; i <= m_slice; ++i) {
    GLfloat s = static_cast<GLfloat>(i) / m_slice;
    m_texcoords.push_back(glm::vec2(s, 1));
  }

  assert(m_texcoords.size() == m_vertices.size());
}

//--------------------------------------------------------------------
void cylinder::draw(GLuint primcount /* = 1*/) {
  GLuint num_vert_bottom = m_slice + 2;  // pole + slice + 1
  GLuint num_vert_center = (m_slice + 1) * 2;

  bind_vertex_array_object();

  if (primcount == 1) {
    glDrawArrays(GL_TRIANGLE_FAN, 0, num_vert_bottom);

    GLuint next = num_vert_bottom;
    for (int i = 0; i < m_stack; ++i, next += num_vert_center) {
      glDrawArrays(GL_TRIANGLE_STRIP, next, num_vert_center);
    }
    glDrawArrays(GL_TRIANGLE_FAN, next, num_vert_bottom);
  } else {
    glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, num_vert_bottom, primcount);

    GLuint next = num_vert_bottom;
    for (int i = 0; i < m_stack - 2; ++i, next += num_vert_center) {
      glDrawArraysInstanced(
        GL_TRIANGLE_STRIP, next, num_vert_center, primcount);
    }
    glDrawArraysInstanced(GL_TRIANGLE_FAN, next, num_vert_bottom, primcount);
  }
}
}
