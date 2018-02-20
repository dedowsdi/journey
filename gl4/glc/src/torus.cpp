#include "torus.h"
#include "common.h"

namespace zxd {

//--------------------------------------------------------------------
GLint torus::build_vertex() {
  GLuint ring_size = (m_sides + 1) * 2;
  GLfloat pi2 = glm::pi<GLfloat>() * 2;
  GLfloat theta_step = pi2 / m_rings;
  GLfloat phi_step = pi2 / m_sides;

  vec3_array& vertices = *(new vec3_array());
  attrib_array(num_arrays(), array_ptr(&vertices));
  vertices.reserve(ring_size * m_rings);

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

      vertices.push_back(v0);
      vertices.push_back(v1);
    }
  }
  return num_arrays() - 1;
}

//--------------------------------------------------------------------
GLint torus::build_normal() {
  vec3_array& normals = *(new vec3_array());
  attrib_array(num_arrays(), array_ptr(&normals));
  normals.reserve(num_vertices());

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

      normals.push_back(glm::normalize(n0));
      normals.push_back(glm::normalize(n1));
    }
  }

  assert(normals.size() == num_vertices());
  return num_arrays() - 1;
}

//--------------------------------------------------------------------
GLint torus::build_texcoord() {
  GLfloat pi2 = glm::pi<GLfloat>() * 2;
  GLfloat theta_step = pi2 / m_rings;
  GLfloat phi_step = pi2 / m_sides;

  vec2_array& texcoords = *(new vec2_array());
  attrib_array(num_arrays(), array_ptr(&texcoords));
  texcoords.reserve(num_vertices());

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

      texcoords.push_back(t0);
      texcoords.push_back(t1);
    }
  }

  assert(texcoords.size() == num_vertices());
  return num_arrays() - 1;
}

//--------------------------------------------------------------------
void torus::draw(GLuint primcount /* = 1*/) {
  bind_vao();

  GLuint ring_size = (m_sides + 1) * 2;

  for (int i = 0; i < m_rings; ++i) {
    draw_arrays(GL_TRIANGLE_STRIP, ring_size * i, ring_size, primcount);
  }
}
}
