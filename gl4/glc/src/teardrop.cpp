#include <teardrop.h>

#include <memory>

#include <glm/trigonometric.hpp>
#include <geometry_util.h>

namespace zxd
{

//--------------------------------------------------------------------
teardrop::teardrop(
  GLfloat radius, GLuint slice, GLuint stack, GLfloat xy_scale /* = 0.5f*/)
    : m_radius(radius), m_slice(slice), m_stack(stack), m_xy_scale(xy_scale)
{
}

//--------------------------------------------------------------------
common_geometry::vertex_build teardrop::build_vertices()
{
  GLuint circle_size = m_slice + 1;
  GLuint strip_size = circle_size * 2;

  auto vertices = std::make_shared<vec3_array>();

  vec3_vector sphere_points =
    get_points(m_radius, m_slice, m_stack, m_xy_scale);
  GLint stack_size = m_slice + 1;
  // create teardrop stack by stack along z
  // build triangle strip as
  //    0 2
  //    1 3
  // pole strip will not be created as triangle fan, as it mess up texture
  for (int i = 0; i < m_stack; ++i)
  {
    GLint stack_start = stack_size * i;
    GLuint next_stack_start = stack_start + stack_size;

    for (int j = 0; j <= m_slice; j++)
    {
      // loop last stack in reverse order
      vertices->push_back(sphere_points[stack_start + j]);
      vertices->push_back(sphere_points[next_stack_start + j]);
    }
  }

  clear_primitive_sets();
  for (int i = 0; i < m_stack; ++i)
    add_primitive_set(std::make_shared<draw_arrays>(
      GL_TRIANGLE_STRIP, strip_size * i, strip_size));
  return vertex_build{vertices};
}

//--------------------------------------------------------------------
array_ptr teardrop::build_texcoords(const array& vertices)
{
  // t ranges from 0.0 at z = - radius to 1.0 at z = radius (t increases
  // linearly along longitudinal lines), and s ranges from 0.0 at the +y axis,
  // to 0.25 at the +x axis, to 0.5 at the \-y axis, to 0.75 at the \-x axis,
  // and back to 1.0 at the +y axis
  //
  auto texcoords = std::make_unique<vec2_array>();

  for (int i = 0; i < m_stack; ++i)
  {
    GLfloat t0 = 1 - static_cast<GLfloat>(i) / m_stack;
    GLfloat t1 = 1 - static_cast<GLfloat>(i + 1) / m_stack;

    for (int j = 0; j <= m_slice; j++)
    {
      GLfloat s = static_cast<GLfloat>(j) / m_slice;

      texcoords->push_back(glm::vec2(s, t0));
      texcoords->push_back(glm::vec2(s, t1));
    }
  }

  assert(texcoords->size() == vertices.size());
  return texcoords;
}

//--------------------------------------------------------------------
vec3_vector teardrop::get_points(
  GLfloat radius, GLuint slices, GLuint stacks, GLfloat xy_scale)
{
  vec3_vector points;
  points.reserve((stacks + 1) * (slices + 1));
  GLfloat phi_step = fpi / stacks;
  GLfloat theta_step = f2pi / slices;
  for (int i = 0; i <= stacks; ++i)
  {
    GLfloat phi = i * phi_step;

    GLfloat sin_phi = std::sin(phi);
    GLfloat cos_phi = std::cos(phi);
    GLfloat z = radius * cos_phi;

    GLfloat sf = xy_scale * (1 - cos_phi) * sin_phi * radius;

    for (int j = 0; j <= slices; j++)
    {
      // loop last stack in reverse order
      GLfloat theta = theta_step * j;
      if(j == slices) theta = 0; // avoid precision problem

      GLfloat cos_theta = std::cos(theta);
      GLfloat sin_theta = std::sin(theta);

      points.push_back(vec3(sf * cos_theta, sf * sin_theta, z));
    }
  }

  return points;
}

}
