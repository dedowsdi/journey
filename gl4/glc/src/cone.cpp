#include "cone.h"
#include "common.h"

namespace zxd {

//--------------------------------------------------------------------
void cone::build_vertex() {
  GLfloat theta_step = f2pi / m_slice;  // azimuthal angle step
  GLfloat height_step = m_height / m_stack;
  GLfloat radius_step = m_radius / m_stack;

  GLuint circle_size = m_slice + 1;
  GLuint strip_size = circle_size * 2;

  vec3_array& vertices = *(new vec3_array());
  attrib_array(num_arrays(), array_ptr(&vertices));
  vertices.reserve(m_stack * strip_size + m_slice + 2);

  // create bottom
  vertices.push_back(vec3(0));
  for (int i = 0; i <= m_slice; ++i) 
  {
    GLfloat angle = -theta_step * i; // in c order
    if(i == m_slice)
      angle = 0;
    vertices.push_back(vec3(cos(angle), sin(angle), 0));
  }

  // create stack from bottom to top, different from sphere.
  // build triangle strip as
  //    0 2
  //    1 3
  for (int i = 0; i < m_stack; ++i) {
    GLfloat h0 = height_step * i;
    GLfloat h1 = h0 + height_step;
    GLfloat r0 = radius_step * (m_stack - i);
    GLfloat r1 = r0 - radius_step;

    for (int j = 0; j <= m_slice; j++) {
      // loop last stack in reverse order
      GLfloat theta = theta_step * j;
      if(j == m_slice)
        theta = 0;
      GLfloat cos_theta = std::cos(theta);
      GLfloat sin_theta = std::sin(theta);

      vertices.push_back(vec3(r1 * cos_theta, r1 * sin_theta, h1));
      vertices.push_back(vec3(r0 * cos_theta, r0 * sin_theta, h0));
    }
  }

  m_primitive_sets.clear();
  add_primitive_set(new draw_arrays(GL_TRIANGLE_FAN, 0, m_slice+2));
  for (int i = 0; i < m_stack; ++i)
    add_primitive_set(new draw_arrays(GL_TRIANGLE_STRIP, m_slice+2 + strip_size * i, strip_size));
}

//--------------------------------------------------------------------
void cone::build_normal() {
  vec3_array& normals = *(new vec3_array());
  attrib_array(num_arrays(), array_ptr(&normals));
  normals.reserve(num_vertices());
  const vec3_array& vertices = *attrib_vec3_array(0);

  GLuint num_vert_bottom = m_slice + 2;

  // normals for bottom
  for (int i = 0; i < num_vert_bottom; ++i) {
    normals.push_back(vec3(0, 0, -1));
  }

  glm::vec3 apex(0, 0, m_height);

  // normals for 1st stack
  for (int i = 0; i <= m_slice; ++i) {
    const vec3& vertex = vertices[num_vert_bottom + i * 2];
    vec3 outer = glm::cross(vertex, apex);
    vec3 normal = glm::cross(apex - vertex, outer);
    normals.push_back(normal);
    normals.push_back(normal);
  }

  for (int i = 1; i < m_stack - 1; ++i) {
    // no reallocate will happen here
    normals.insert(normals.end(), normals.begin() + num_vert_bottom,
      normals.begin() + num_vert_bottom + (m_slice + 1) * 2);
  }

  // normals for last stacks
  for (int i = 0; i <= m_slice; ++i) {
    normals.push_back(vec3(0, 0, 1));
    normals.push_back(normals[num_vert_bottom + i * 2]);
  }

  assert(normals.size() == num_vertices());
}

//--------------------------------------------------------------------
void cone::build_texcoord() {
  vec2_array& texcoords = *(new vec2_array());
  attrib_array(num_arrays(), array_ptr(&texcoords));
  texcoords.reserve(num_vertices());

  // bottom texcoords
  texcoords.push_back(glm::vec2(0, 0));
  for (int i = 0; i <= m_slice; ++i) {
    GLfloat s = 1 - static_cast<GLfloat>(i) / m_slice;
    texcoords.push_back(glm::vec2(s, 0));
  }

  for (int i = 0; i < m_stack; ++i) {
    GLfloat t0 = static_cast<GLfloat>(i + 1) / m_stack;
    GLfloat t1 = static_cast<GLfloat>(i) / m_stack;


    for (int j = 0; j <= m_slice; j++) {
      // loop last stack in reverse order
      GLfloat s = static_cast<GLfloat>(j) / m_slice;

      texcoords.push_back(glm::vec2(s, t0));
      texcoords.push_back(glm::vec2(s, t1));
    }
  }

  assert(texcoords.size() == num_vertices());
}

}
