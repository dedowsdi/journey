#include "torus.h"
#include "common.h"

namespace zxd {

//--------------------------------------------------------------------
void torus::build_vertex() {
  GLuint ring_size = (m_sides + 1) * 2;
  GLfloat pi2 = glm::pi<GLfloat>() * 2;
  GLfloat theta_step = pi2 / m_rings;
  GLfloat phi_step = pi2 / m_sides;

  m_vertices.clear();
  m_vertices.reserve(ring_size * m_rings);

  // build torus ring by ring(triangle strip), use theta along +z to rotate
  // ring, use phi along -y to rotate vertex in ring.
  // see image in http://paulbourke.net/geometry/torus/
  for (int i = 0; i < m_rings; ++i) {
    GLfloat theta0 = theta_step * i;
    GLfloat theta1 = theta0 + theta_step;
    GLfloat cos_theta0 = glm::cos(theta0);
    GLfloat cos_theta1 = glm::cos(theta1);
    GLfloat sin_theta0 = glm::sin(theta0);
    GLfloat sin_theta1 = glm::sin(theta1);

    for (int j = 0; j <= m_sides; ++j) {
      GLfloat phi = phi_step * j;

      // the first circle resides on xz plane, projection of vertex of this
      // circle on xy plane is (0, 0, r), rotate it along -y, and you get the
      // xy part of final vertex
      GLfloat r = m_outer_radius + m_inner_radius * glm::cos(phi);
      vec3 v0(cos_theta0 * r, sin_theta0 * r, m_inner_radius * glm::sin(phi));
      vec3 v1(cos_theta1 * r, sin_theta1 * r, m_inner_radius * glm::sin(phi));

      m_vertices.push_back(v0);
      m_vertices.push_back(v1);
    }
  }
}

//--------------------------------------------------------------------
void torus::build_normal() {
  m_normals.clear();
  m_normals.reserve(m_vertices.capacity());

  GLfloat pi2 = glm::pi<GLfloat>() * 2;
  GLfloat theta_step = pi2 / m_rings;
  GLfloat phi_step = pi2 / m_sides;

  for (int i = 0; i < m_rings; ++i) {
    GLfloat theta0 = theta_step * i;
    GLfloat theta1 = theta0 + theta_step;
    GLfloat cos_theta0 = glm::cos(theta0);
    GLfloat cos_theta1 = glm::cos(theta1);
    GLfloat sin_theta0 = glm::sin(theta0);
    GLfloat sin_theta1 = glm::sin(theta1);

    for (int j = 0; j <= m_sides; ++j) {
      GLfloat phi = phi_step * j;

      GLfloat r = m_outer_radius + m_inner_radius * glm::cos(phi);
      vec3 v0(cos_theta0 * r, sin_theta0 * r, m_inner_radius * glm::sin(phi));
      vec3 v1(cos_theta1 * r, sin_theta1 * r, m_inner_radius * glm::sin(phi));

      // normal is the same as vertex - circle center
      vec3 n0 =
        v0 - vec3(cos_theta0 * m_outer_radius, sin_theta0 * m_outer_radius, 0);
      vec3 n1 =
        v1 - vec3(cos_theta1 * m_outer_radius, sin_theta1 * m_outer_radius, 0);

      m_normals.push_back(glm::normalize(n0));
      m_normals.push_back(glm::normalize(n1));
    }
  }

  assert(m_normals.size() == m_vertices.size());
}

//--------------------------------------------------------------------
void torus::build_texcoord() {
  GLfloat pi2 = glm::pi<GLfloat>() * 2;
  GLfloat theta_step = pi2 / m_rings;
  GLfloat phi_step = pi2 / m_sides;

  m_texcoords.clear();
  m_texcoords.reserve(m_vertices.capacity());

  // build torus ring by ring(triangle strip), use theta along +z to rotate
  // ring, use phi along -y to rotate vertex in ring.
  // see image in http://paulbourke.net/geometry/torus/
  for (int i = 0; i < m_rings; ++i) {
    GLfloat theta0 = theta_step * i;
    GLfloat theta1 = theta0 + theta_step;

    for (int j = 0; j <= m_sides; ++j) {
      GLfloat phi = phi_step * j;

      vec2 t0(theta0 / pi2, phi / pi2);
      vec2 t1(theta1 / pi2, phi / pi2);

      m_texcoords.push_back(t0);
      m_texcoords.push_back(t1);
    }
  }

  assert(m_texcoords.size() == m_vertices.size());
}

//--------------------------------------------------------------------
void torus::draw(GLuint primcount /* = 1*/) {
  bind_vertex_array_object();

  GLuint ring_size = (m_sides + 1) * 2;

  if (primcount == 1) {
    for (int i = 0; i < m_rings; ++i) {
      glDrawArrays(GL_TRIANGLE_STRIP, ring_size * i, ring_size);
    }
  } else {
    for (int i = 0; i < m_rings; ++i) {
      glDrawArraysInstanced(
        GL_TRIANGLE_STRIP, ring_size * i, ring_size, primcount);
    }
  }
}
}
