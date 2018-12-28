#include "capsule3d.h"
#include "sphere.h"
#include "geometry_util.h"

namespace zxd
{

//--------------------------------------------------------------------
void capsule3d::build_vertex()
{

  if(m_sphere_slice & 1)
    m_sphere_slice += 1;
  if(m_sphere_stack & 1)
    m_sphere_stack += 1;
  if(m_cylinder_stack & 1)
    m_cylinder_stack += 1;

  auto vertices = std::make_shared<vec3_array>();
  attrib_array(0, vertices);

  vec3_vector sphere_points = sphere::get_sphere_points(m_radius, m_sphere_slice, m_sphere_stack);

  GLint stack_size = m_sphere_slice + 1;
  // create sphere stack by stack along z
  // build triangle strip as
  //    0 2
  //    1 3
  // pole strip will not be created as triangle fan, as it mess up texture
  vec3 offset = vec3(0, 0, m_height * 0.5f - m_radius);
  GLuint equator = m_sphere_stack/2;
  
  // sphere in both ends
  for (int i = 0; i < m_sphere_stack; ++i) {
    GLint stack_start = stack_size * i;
    GLuint next_stack_start = stack_start + stack_size;

    GLfloat sign = i >= equator ? -1 : 1;
    vec3 real_offset = offset * sign;

    for (int j = 0; j <= m_sphere_slice; j++) {
      // loop last stack in reverse order
      vertices->push_back(real_offset + sphere_points[stack_start + j]);
      vertices->push_back(real_offset + sphere_points[next_stack_start + j]);
    }
  }

  vec3_vector bottom;
  vec3_vector up;

  bottom.reserve(m_sphere_slice + 1);
  up.reserve(m_sphere_slice + 1);

  for (int i = 0; i <= m_sphere_slice; ++i) {
    const vec3& v = sphere_points[equator * stack_size + i];
    up.push_back(v + offset);
    bottom.push_back(v - offset);
  }

  // cylinder center
  GLfloat step = 1.0f / m_cylinder_stack;
  for (int i = 0; i < m_cylinder_stack; ++i) 
  {
    for (int j = 0; j <= m_sphere_slice; j++) {
      vertices->push_back(mix(up[j], bottom[j], step * i));
      vertices->push_back(mix(up[j], bottom[j], step * (i+1)));
    }
  }

  m_primitive_sets.clear();
  GLuint circle_size = m_sphere_slice + 1;
  GLuint strip_size = circle_size * 2;
  assert(vertices->size() == strip_size * (m_sphere_stack + m_cylinder_stack));
  for (int i = 0; i < m_sphere_stack + m_cylinder_stack; ++i)
    add_primitive_set(new draw_arrays(GL_TRIANGLE_STRIP, strip_size * i, strip_size));
}

//--------------------------------------------------------------------
void capsule3d::build_normal()
{
  geometry_util::smooth(*this, 1);
}

void capsule3d::build_texcoord()
{
  vec2_array& texcoords = *(new vec2_array());
  attrib_array(num_arrays(), array_ptr(&texcoords));
  texcoords.reserve(num_vertices());

  GLfloat cylinder_end_t = m_radius / m_height;
  GLfloat cylinder_start_t = 1 - cylinder_end_t;

  // texcoords for sphere, the one with max z has t = 1
  GLuint equator = m_sphere_stack/2;
  GLfloat step = 1.0f / equator;
  for (int i = 0; i < m_sphere_stack ; ++i) {

    GLfloat t0, t1;

    if(i < equator)
    {
      t0 = mix(1.0f, cylinder_start_t, i * step);
      t1 = mix(1.0f, cylinder_start_t, (i+1) * step);
    }
    else
    {
      t0 = mix(cylinder_end_t, 0.0f, (i - equator) * step);
      t1 = mix(cylinder_end_t, 0.0f, (i+1 - equator) * step);
    }

    for (int j = 0; j <= m_sphere_slice; j++) {
      GLfloat s = static_cast<GLfloat>(j) / m_sphere_slice;

      texcoords.push_back(glm::vec2(s, t0));
      texcoords.push_back(glm::vec2(s, t1));
    }
  }

  // texcoords for cylinder
  step = 1.0f / m_cylinder_stack;
  for (int i = 0; i < m_cylinder_stack ; ++i) {
    GLfloat t0 = mix(cylinder_start_t, cylinder_end_t, i * step);
    GLfloat t1 = mix(cylinder_start_t, cylinder_end_t, (i+1) * step);

    for (int j = 0; j <= m_sphere_slice; j++) {
      GLfloat s = static_cast<GLfloat>(j) / m_sphere_slice;

      texcoords.push_back(glm::vec2(s, t0));
      texcoords.push_back(glm::vec2(s, t1));

    }
  }

  assert(texcoords.size() == num_vertices());
}


}
