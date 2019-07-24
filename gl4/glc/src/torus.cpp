#include "torus.h"
#include "common.h"

namespace zxd
{

//--------------------------------------------------------------------
void torus::build_vertex()
{
  GLuint ring_size = (m_sides + 1) * 2;
  GLfloat pi2 = glm::pi<GLfloat>() * 2;
  GLfloat phi_step = pi2 / m_sides;
  GLfloat theta_step = pi2 / m_rings;

  vec3_array& vertices = *(new vec3_array());
  attrib_array(num_arrays(), array_ptr(&vertices));
  vertices.reserve(ring_size * m_rings);

  // build torus tube ring by tube ring(triangle strip, ), use phi along +z to
  // rotate ring, use theta along -y to rotate vertex in ring.  see image in
  // http://paulbourke.net/geometry/torus/
  for (int i = 0; i < m_rings; ++i)
  {
    GLfloat phi0 = phi_step * i;
    GLfloat phi1 = phi0 + phi_step;
    GLfloat cos_phi0 = glm::cos(phi0);
    GLfloat cos_phi1 = glm::cos(phi1);
    GLfloat sin_phi0 = glm::sin(phi0);
    GLfloat sin_phi1 = glm::sin(phi1);

    for (int j = 0; j <= m_sides; ++j)
    {
      GLfloat theta = theta_step * j;

      // the first circle resides on xz plane, projection of vertex of this
      // circle on xy plane is (0, 0, r), rotate it along -y, and you get the
      // xy part of final vertex
      GLfloat r = m_outer_radius + m_inner_radius * glm::cos(theta);
      glm::vec3 v0(cos_phi0 * r, sin_phi0 * r, m_inner_radius * glm::sin(theta));
      glm::vec3 v1(cos_phi1 * r, sin_phi1 * r, m_inner_radius * glm::sin(theta));

      vertices.push_back(v0);
      vertices.push_back(v1);
    }
  }

  m_primitive_sets.clear();
  for (int i = 0; i < m_rings; ++i)
  {
    add_primitive_set(new draw_arrays(GL_TRIANGLE_STRIP, ring_size * i, ring_size));
  }
}

//--------------------------------------------------------------------
void torus::build_normal()
{
  vec3_array& normals = *(new vec3_array());
  attrib_array(num_arrays(), array_ptr(&normals));
  normals.reserve(num_vertices());

  GLfloat pi2 = glm::pi<GLfloat>() * 2;
  GLfloat phi_step = pi2 / m_sides;
  GLfloat theta_step = pi2 / m_rings;

  for (int i = 0; i < m_rings; ++i)
  {
    GLfloat phi0 = phi_step * i;
    GLfloat phi1 = phi0 + phi_step;
    GLfloat cos_phi0 = glm::cos(phi0);
    GLfloat cos_phi1 = glm::cos(phi1);
    GLfloat sin_phi0 = glm::sin(phi0);
    GLfloat sin_phi1 = glm::sin(phi1);

    for (int j = 0; j <= m_sides; ++j)
    {
      GLfloat theta = theta_step * j;

      GLfloat r = m_outer_radius + m_inner_radius * glm::cos(theta);
      glm::vec3 v0(cos_phi0 * r, sin_phi0 * r, m_inner_radius * glm::sin(theta));
      glm::vec3 v1(cos_phi1 * r, sin_phi1 * r, m_inner_radius * glm::sin(theta));

      // normal is the same as vertex - circle center
      glm::vec3 n0 =
        v0 - vec3(cos_phi0 * m_outer_radius, sin_phi0 * m_outer_radius, 0);
      glm::vec3 n1 =
        v1 - vec3(cos_phi1 * m_outer_radius, sin_phi1 * m_outer_radius, 0);

      normals.push_back(glm::normalize(n0));
      normals.push_back(glm::normalize(n1));
    }
  }

  assert(normals.size() == num_vertices());
}

//--------------------------------------------------------------------
void torus::build_texcoord()
{
  GLfloat pi2 = glm::pi<GLfloat>() * 2;
  GLfloat phi_step = pi2 / m_sides;
  GLfloat theta_step = pi2 / m_rings;

  vec2_array& texcoords = *(new vec2_array());
  attrib_array(num_arrays(), array_ptr(&texcoords));
  texcoords.reserve(num_vertices());

  // build torus ring by ring(triangle strip), use phi along +z to rotate
  // ring, use theta along -y to rotate vertex in ring.
  // see image in http://paulbourke.net/geometry/torus/
  for (int i = 0; i < m_rings; ++i)
  {
    GLfloat phi0 = phi_step * i;
    GLfloat phi1 = phi0 + phi_step;

    for (int j = 0; j <= m_sides; ++j)
    {
      GLfloat theta = theta_step * j;

      glm::vec2 t0(phi0 / pi2, theta / pi2);
      glm::vec2 t1(phi1 / pi2, theta / pi2);

      texcoords.push_back(t0);
      texcoords.push_back(t1);
    }
  }

  assert(texcoords.size() == num_vertices());
}

}
