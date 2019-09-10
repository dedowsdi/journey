#include "sphere.h"

#include <memory>
#include <algorithm>

#include <glm/geometric.hpp>
#include "geometry_util.h"

namespace zxd
{

sphere::sphere(GLfloat radius, GLuint slice, GLuint stack)
    : m_radius(radius), m_slice(slice), m_stack(stack)
{
}

//--------------------------------------------------------------------
common_geometry::vertex_build sphere::build_vertices()
{
  vec3_vector sphere_points = create_points(m_radius, m_slice, m_stack);

  auto vertices = std::make_unique<vec3_array>();
  vertices->assign(sphere_points.begin(), sphere_points.end());

  auto& elements = make_element<uint_array>();

  // reserve extra space for primitive restart index
  auto num_elements = m_stack * 2 * (m_slice + 1) + m_stack;
  elements.reserve(num_elements);
  build_strip_elements(elements, m_stack, m_slice);

  assert(elements.size() == num_elements);

  clear_primitive_sets();
  add_primitive_set(std::make_shared<draw_elements>(
    GL_TRIANGLE_STRIP, elements.size(), GL_UNSIGNED_INT, 0));
  return vertex_build(std::move(vertices));
}

//--------------------------------------------------------------------
array_uptr sphere::build_normals(const array& vertices)
{
  auto normals = std::make_unique<vec3_array>();
  auto& v = static_cast<const vec3_array&>(vertices);

  std::transform(v.begin(), v.end(), std::back_inserter(*normals),
    [](auto &pos) -> vec3 { return normalize(pos); });
  return normals;
}

//--------------------------------------------------------------------
array_uptr sphere::build_texcoords(const array& vertices)
{
  auto texcoords = std::make_unique<vec2_array>();
  texcoords->reserve(vertices.size());

  // t ranges from 0.0 at z = - radius to 1.0 at z = radius (t increases
  // linearly along longitudinal lines), and s ranges from 0.0 at the +x axis,
  // to 0.25 at the +y axis, to 0.5 at the -x axis, to 75 at the -y axis,
  // and back to 1.0 at the +x axis
  build_strip_texcoords(*texcoords, m_stack, m_slice, 1, 0);

  assert(texcoords->size() == vertices.size());
  return texcoords;
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
