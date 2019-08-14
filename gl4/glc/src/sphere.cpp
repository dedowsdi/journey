#include "sphere.h"

#include <memory>
#include <algorithm>

namespace zxd
{

//--------------------------------------------------------------------
void sphere::build_vertex()
{
  vec3_vector sphere_points = create_points(m_radius, m_slice, m_stack);

  auto vertices = make_array<vec3_array>(0);
  vertices->assign(sphere_points.begin(), sphere_points.end());

  m_elements = std::make_shared<uint_array>();
  auto elements = std::static_pointer_cast<uint_array>(m_elements);

  // reserve extra space for primitive restart index
  elements->reserve(m_stack * 2 * (m_slice + 2));

  // create sphere stack by stack along z, from up to down
  // build triangle strip (front face) as
  //    0 2
  //    1 3
  for (int i = 0; i < m_stack; ++i)
  {
    auto stack0 = (m_slice + 1) * i;
    auto stack1 = stack0 + m_slice + 1;

    for (int j = 0; j <= m_slice; j++)
    {
      elements->push_back(stack0 + j);
      elements->push_back(stack1 + j);
    }
    elements->push_back(-1);
  }

  m_primitive_sets.clear();
  add_primitive_set(
    new draw_elements(GL_TRIANGLE_STRIP, elements->size(), GL_UNSIGNED_INT, 0));
}

//--------------------------------------------------------------------
void sphere::build_normal()
{
  auto normals = make_array<vec3_array>(num_arrays());
  auto vertices = attrib_vec3_array(0);

  std::transform(vertices->begin(), vertices->end(),
    std::back_inserter(*normals),
    [](auto &pos) -> vec3 { return normalize(pos); });
}

//--------------------------------------------------------------------
void sphere::build_texcoord()
{
  auto texcoords = make_array<vec2_array>(num_arrays());
  texcoords->reserve(num_vertices());

  // t ranges from 0.0 at z = - radius to 1.0 at z = radius (t increases
  // linearly along longitudinal lines), and s ranges from 0.0 at the +y axis,
  // to 0.25 at the +x axis, to 0.5 at the \-y axis, to 0.75 at the \-x axis,
  // and back to 1.0 at the +y axis
  for (int i = 0; i <= m_stack; ++i)
  {
    GLfloat t = 1 - static_cast<GLfloat>(i) / m_stack;
    for (int j = 0; j <= m_slice; j++)
    {
      GLfloat s = static_cast<GLfloat>(j) / m_slice;
      texcoords->push_back(vec2(s, t));
    }
  }

  assert(texcoords->size() == num_vertices());
}

//--------------------------------------------------------------------
vec3_vector sphere::create_points(GLfloat radius, GLuint slices, GLuint stacks)
{
  // use iso convension:
  //    θ : polar angle
  //    φ : azimuthal angle

  vec3_vector sphere_point;
  sphere_point.reserve((stacks + 1) * (slices + 1));

  GLfloat theta_step = zxd::fpi / stacks;
  GLfloat phi_step = f2pi / slices;

  for (int i = 0; i <= stacks; ++i)
  {
    GLfloat theta = i * theta_step;
    GLfloat sin_theta = std::sin(theta);
    GLfloat cos_theta = std::cos(theta);

    for (int j = 0; j <= slices; j++)
    {
      GLfloat phi = j == slices ? 0 : phi_step * j;
      sphere_point.push_back(
        radius * vec3(sin_theta * cos(phi), sin_theta * sin(phi), cos_theta));
    }
  }

  return sphere_point;
}

//--------------------------------------------------------------------
void sphere::on_draw()
{
  glEnable(GL_PRIMITIVE_RESTART_FIXED_INDEX);
}

}
