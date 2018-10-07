#include "sphere.h"
#include "common.h"
#include <memory>

namespace zxd {

//--------------------------------------------------------------------
void sphere::build_vertex() {
  GLfloat phi_step = glm::pi<GLfloat>() / m_stack;
  GLfloat theta_step = 2 * glm::pi<GLfloat>() / m_slice;

  GLuint num_vert_pole = m_slice + 1;
  GLuint num_vert_center = (m_slice + 1) * 2;

  vec3_array& vertices = *(new vec3_array());
  attrib_array(num_arrays(), array_ptr(&vertices));
  vertices.reserve(num_vert_pole * 2 + num_vert_center * (m_stack - 2));

  // create sphere stack by stack along z
  // build triangle strip as
  //    0 2
  //    1 3
  for (int i = 0; i < m_stack; ++i) {
    GLfloat phi0 = i * phi_step;
    GLfloat phi1 = phi0 + phi_step;

    GLfloat sin_phi0 = std::sin(phi0);
    GLfloat sin_phi1 = std::sin(phi1);
    GLfloat cos_phi0 = std::cos(phi0);
    GLfloat cos_phi1 = std::cos(phi1);

    GLfloat r_times_sin_phi0 = m_radius * sin_phi0;
    GLfloat r_times_sin_phi1 = m_radius * sin_phi1;
    GLfloat r_times_cos_phi0 = m_radius * cos_phi0;
    GLfloat r_times_cos_phi1 = m_radius * cos_phi1;

    // add pole
    if (i == 0) vertices.push_back(glm::vec3(0, 0, m_radius));
    if (i == m_stack - 1) vertices.push_back(glm::vec3(0, 0, -m_radius));

    for (int j = 0; j <= m_slice; j++) {
      // loop last stack in reverse order
      GLfloat theta = theta_step * (i == m_stack - 1 ? -j : j);
      GLfloat cos_theta = std::cos(theta);
      GLfloat sin_theta = std::sin(theta);

      // x = rho * sin(phi) * cos(theta)
      // x = rho * sin(phi) * sin(theta)
      // z = rho * cos(phi)
      if (i != 0)
        vertices.push_back(vec3(r_times_sin_phi0 * cos_theta,
          r_times_sin_phi0 * sin_theta, r_times_cos_phi0));

      if (i != m_stack - 1)
        vertices.push_back(vec3(r_times_sin_phi1 * cos_theta,
          r_times_sin_phi1 * sin_theta, r_times_cos_phi1));
    }
  }
}

//--------------------------------------------------------------------
void sphere::build_normal() {
  vec3_array& normals = *(new vec3_array());
  attrib_array(num_arrays(), array_ptr(&normals));
  const vec3_array& vertices = *attrib_vec3_array(0);

  normals.reserve(num_vertices());

  for (int i = 0; i < num_vertices(); ++i) {
    normals.push_back(glm::normalize(vertices[i]));
  }
}

//--------------------------------------------------------------------
void sphere::build_texcoord() {
  // t ranges from 0.0 at z = - radius to 1.0 at z = radius (t increases
  // linearly along longitudinal lines), and s ranges from 0.0 at the +y axis,
  // to 0.25 at the +x axis, to 0.5 at the \-y axis, to 0.75 at the \-x axis,
  // and back to 1.0 at the +y axis
  //
  vec2_array& texcoords = *(new vec2_array());
  attrib_array(num_arrays(), array_ptr(&texcoords));
  texcoords.reserve(num_vertices());

  for (int i = 0; i < m_stack; ++i) {
    GLfloat t0 = 1 - static_cast<GLfloat>(i) / m_stack;
    GLfloat t1 = 1 - static_cast<GLfloat>(i + 1) / m_stack;

    // add pole
    if (i == 0) texcoords.push_back(glm::vec2(1, 0));
    if (i == m_stack - 1) texcoords.push_back(glm::vec2(0, 0));

    for (int j = 0; j <= m_slice; j++) {
      // loop last stack in reverse order
      GLfloat s = static_cast<GLfloat>(j) / m_slice;
      if (i == m_stack - 1) s = 1 - s;

      if (i != 0) texcoords.push_back(glm::vec2(s, t0));

      if (i != m_stack - 1) texcoords.push_back(glm::vec2(s, t1));
    }
  }

  assert(texcoords.size() == num_vertices());
}

//--------------------------------------------------------------------
void sphere::draw(GLuint primcount /* = 1*/) {
  GLuint num_vert_pole = m_slice + 2;  // pole + slice + 1
  GLuint num_vert_center = (m_slice + 1) * 2;

  bind_vao();

  draw_arrays(GL_TRIANGLE_FAN, 0, num_vert_pole, primcount);

  GLuint next = num_vert_pole;
  for (int i = 0; i < m_stack - 2; ++i, next += num_vert_center) {
    draw_arrays(GL_TRIANGLE_STRIP, next, num_vert_center, primcount);
  }
  draw_arrays(GL_TRIANGLE_FAN, next, num_vert_pole, primcount);
}

}
