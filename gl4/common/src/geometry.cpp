#include "geometry.h"
#include "common.h"

namespace zxd {

//--------------------------------------------------------------------
void geometry::bind(
  GLint vertex, GLint normal /* = -1*/, GLint texcoord /* = -1*/) {
  bind_vertex_array_object();

  if (vertex != -1) {
    glGenBuffers(1, &m_vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(glm::vec3),
      value_ptr(m_vertices[0]), GL_STATIC_DRAW);

    glVertexAttribPointer(vertex, 3, GL_FLOAT, 0, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(vertex);
  }

  if (normal != -1) {
    glGenBuffers(1, &m_normal_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_normal_buffer);
    glBufferData(GL_ARRAY_BUFFER, m_normals.size() * sizeof(glm::vec3),
      value_ptr(m_normals[0]), GL_STATIC_DRAW);

    glVertexAttribPointer(normal, 3, GL_FLOAT, 0, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(normal);
  }

  if (texcoord != -1) {
    glGenBuffers(1, &m_texcoord_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_texcoord_buffer);
    glBufferData(GL_ARRAY_BUFFER, m_texcoords.size() * sizeof(glm::vec2),
      value_ptr(m_texcoords[0]), GL_STATIC_DRAW);

    glVertexAttribPointer(texcoord, 2, GL_FLOAT, 0, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(texcoord);
  }
}

//--------------------------------------------------------------------
void geometry::build_mesh(
  GLboolean normal /* = 1*/, GLboolean texcoord /* = 1*/) {
  build_vertex();
  if (normal) {
    build_normal();
  }
  if (texcoord) {
    build_texcoord();
  }
}

//--------------------------------------------------------------------
void geometry::bind_vertex_array_object() {
  if (m_vao == -1) {
    glGenVertexArrays(1, &m_vao);
  }
  glBindVertexArray(m_vao);
}
}
