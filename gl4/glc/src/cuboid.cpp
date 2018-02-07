#include "cuboid.h"
#include "common.h"

namespace zxd {

//--------------------------------------------------------------------
void cuboid::build_vertex() {
  m_vertices.clear();
  m_vertices.reserve(24);

  GLfloat x = m_half_diag.x;
  GLfloat y = m_half_diag.y;
  GLfloat z = m_half_diag.z;
  GLfloat x2 = x * 2;
  GLfloat y2 = y * 2;
  GLfloat z2 = z * 2;

  // make sure that if  rotate around z, origin of every face is at low left
  // corner.
  glm::vec3 origins[6] = {
    vec3(-x, -y, -z),  // front
    vec3(x, y, -z),    // back
    vec3(x, -y, -z),   // right
    vec3(-x, y, -z),   // left
    vec3(-x, -y, z),   // top
    vec3(-x, y, -z)    // bottom
  };

  glm::vec3 xsteps[6] = {
    vec3(x2, 0, 0), vec3(-x2, 0, 0),  // front, back
    vec3(0, y2, 0), vec3(0, -y2, 0),  // right, left
    vec3(x2, 0, 0), vec3(x2, 0, 0)    // top, bottom
  };

  glm::vec3 ysteps[6] = {
    vec3(0, 0, z2), vec3(0, 0, z2),  // front, back
    vec3(0, 0, z2), vec3(0, 0, z2),  // right, left
    vec3(0, y2, 0), vec3(0, -y2, 0)  // top, bottom
  };

  for (int i = 0; i < 6; ++i) {
    const vec3& origin = origins[i];
    const vec3& xstep = xsteps[i];
    const vec3& ystep = ysteps[i];

    m_vertices.push_back(origin);
    m_vertices.push_back(origin + xstep);
    m_vertices.push_back(origin + xstep + ystep);

    m_vertices.push_back(origin);
    m_vertices.push_back(origin + xstep + ystep);
    m_vertices.push_back(origin + ystep);
  }
}

//--------------------------------------------------------------------
void cuboid::build_normal() {
  m_normals.clear();
  m_normals.reserve(m_vertices.size());
  for (int i = 0; i < 6; ++i) m_normals.push_back(glm::vec3(0, -1, 0));
  for (int i = 0; i < 6; ++i) m_normals.push_back(glm::vec3(0, 1, 0));
  for (int i = 0; i < 6; ++i) m_normals.push_back(glm::vec3(1, 0, 0));
  for (int i = 0; i < 6; ++i) m_normals.push_back(glm::vec3(-1, 0, 0));
  for (int i = 0; i < 6; ++i) m_normals.push_back(glm::vec3(0, 0, 1));
  for (int i = 0; i < 6; ++i) m_normals.push_back(glm::vec3(0, 0, -1));
}

//--------------------------------------------------------------------
void cuboid::build_texcoord() {
  m_texcoords.clear();
  m_texcoords.reserve(m_vertices.size());

  for (int i = 0; i < 6; ++i) {
    m_texcoords.push_back(glm::vec2(0, 0));
    m_texcoords.push_back(glm::vec2(1, 0));
    m_texcoords.push_back(glm::vec2(1, 1));
    m_texcoords.push_back(glm::vec2(0, 0));
    m_texcoords.push_back(glm::vec2(1, 1));
    m_texcoords.push_back(glm::vec2(0, 1));
  }
}

//--------------------------------------------------------------------
void cuboid::draw(GLuint primcount /* = 1*/) {
  bind_vertex_array_object();
  draw_arrays(GL_TRIANGLES, 0, m_vertices.size(), primcount);
}
}
