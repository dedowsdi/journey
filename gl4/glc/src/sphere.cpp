#include "sphere.h"
#include "common.h"
#include <memory>

namespace zxd {

//--------------------------------------------------------------------
void sphere::build_vertex() {

  GLuint circle_size = m_slice + 1;
  GLuint strip_size = circle_size * 2;

  vec3_array& vertices = *(new vec3_array());
  attrib_array(num_arrays(), array_ptr(&vertices));
  vertices.reserve((m_stack * strip_size));

  vec3_vector sphere_points = get_sphere_points(m_radius, m_slice, m_stack);
  GLint stack_size = m_slice + 1;
  // create sphere stack by stack along z
  // build triangle strip as
  //    0 2
  //    1 3
  // pole strip will not be created as triangle fan, as it mess up texture
  for (int i = 0; i < m_stack; ++i) {
    GLint stack_start = stack_size * i;
    GLuint next_stack_start = stack_start + stack_size;

    for (int j = 0; j <= m_slice; j++) {
      // loop last stack in reverse order
      vertices.push_back(sphere_points[stack_start + j]);
      vertices.push_back(sphere_points[next_stack_start + j]);
    }
  }

  m_primitive_sets.clear();
  for (int i = 0; i < m_stack; ++i)
    add_primitive_set(new draw_arrays(GL_TRIANGLE_STRIP, strip_size * i, strip_size));
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

    for (int j = 0; j <= m_slice; j++) {
      // loop last stack in reverse order
      GLfloat s = static_cast<GLfloat>(j) / m_slice;
      if (i == m_stack - 1) s = 1 - s;

      texcoords.push_back(glm::vec2(s, t0));
      texcoords.push_back(glm::vec2(s, t1));
    }
  }

  assert(texcoords.size() == num_vertices());
}

//--------------------------------------------------------------------
vec3_vector sphere::get_sphere_points(GLfloat radius, GLuint slices, GLuint stacks)
{
  vec3_vector sphere_point;
  sphere_point.reserve((stacks + 1) * (slices + 1));
  GLfloat phi_step = glm::pi<GLfloat>() / stacks;
  GLfloat theta_step = f2pi / slices;
  for (int i = 0; i <= stacks; ++i) {
    GLfloat phi = i * phi_step;

    GLfloat sin_phi = std::sin(phi);
    GLfloat cos_phi = std::cos(phi);

    GLfloat r_times_sin_phi = radius * sin_phi;
    GLfloat r_times_cos_phi = radius * cos_phi;

    for (int j = 0; j <= slices; j++) {
      // loop last stack in reverse order
      GLfloat theta = theta_step * j;
      if(j == slices) theta = 0; // avoid precision problem

      GLfloat cos_theta = std::cos(theta);
      GLfloat sin_theta = std::sin(theta);

      sphere_point.push_back(vec3(r_times_sin_phi * cos_theta,
            r_times_sin_phi * sin_theta, r_times_cos_phi));
    }
  }

  return sphere_point;
}

}
