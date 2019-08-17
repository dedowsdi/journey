#include "sphere.h"

#include <memory>
#include <algorithm>

#include "geometry_util.h"

namespace zxd
{

//--------------------------------------------------------------------
void sphere::build_vertex()
{
  vec3_vector sphere_points = create_points(m_radius, m_slice, m_stack);

  auto vertices = make_array<vec3_array>(0);
  vertices->assign(sphere_points.begin(), sphere_points.end());

  auto elements = make_element<uint_array>();

  // reserve extra space for primitive restart index
  auto num_elements = m_stack * 2 * (m_slice + 1) + m_stack;
  elements->reserve(num_elements);
  build_strip_elements(*elements, m_stack, m_slice);

  assert(elements->size() == num_elements);

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
  // linearly along longitudinal lines), and s ranges from 0.0 at the +x axis,
  // to 0.25 at the +y axis, to 0.5 at the -x axis, to 75 at the -y axis,
  // and back to 1.0 at the +x axis
  build_strip_texcoords(*texcoords, m_stack, m_slice, 1, 0);

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

}
