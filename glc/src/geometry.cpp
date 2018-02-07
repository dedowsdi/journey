#include "geometry.h"
#include "common.h"

namespace zxd {

//--------------------------------------------------------------------
void geometry::bind(GLint vertex, GLint normal /* = -1*/,
  GLint texcoord /* = -1*/, GLint tangent /* = -1*/) {
  bind_vertex_array_object();

  if (vertex != -1) {
    glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
    glVertexAttribPointer(vertex, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(vertex);
  }

  if (normal != -1) {
    glBindBuffer(GL_ARRAY_BUFFER, m_normal_buffer);
    glVertexAttribPointer(normal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(normal);
  }

  if (texcoord != -1) {
    glBindBuffer(GL_ARRAY_BUFFER, m_texcoord_buffer);
    glVertexAttribPointer(texcoord, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(texcoord);
  }

  if (tangent != -1) {
    glBindBuffer(GL_ARRAY_BUFFER, m_tangent_buffer);
    glVertexAttribPointer(tangent, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(tangent);
  }
}

//--------------------------------------------------------------------
void geometry::build_mesh(GLboolean normal /* = 1*/,
  GLboolean texcoord /* = 1*/, GLboolean tangent /* = -1*/) {
  build_vertex();
  glGenBuffers(1, &m_vertex_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
  glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(glm::vec3),
    value_ptr(m_vertices[0]), GL_STATIC_DRAW);

  if (normal) {
    build_normal();
    glGenBuffers(1, &m_normal_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_normal_buffer);
    glBufferData(GL_ARRAY_BUFFER, m_normals.size() * sizeof(glm::vec3),
      value_ptr(m_normals[0]), GL_STATIC_DRAW);
  }
  if (texcoord) {
    build_texcoord();
    glGenBuffers(1, &m_texcoord_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_texcoord_buffer);
    glBufferData(GL_ARRAY_BUFFER, m_texcoords.size() * sizeof(glm::vec2),
      value_ptr(m_texcoords[0]), GL_STATIC_DRAW);
  }
  if (tangent) {
    build_tangent();
    glGenBuffers(1, &m_tangent_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_tangent_buffer);
    glBufferData(GL_ARRAY_BUFFER, m_tangents.size() * sizeof(glm::vec3),
      value_ptr(m_tangents[0]), GL_STATIC_DRAW);
  }
}

//--------------------------------------------------------------------
void geometry::draw_arrays(
  GLenum mode, GLint first, GLsizei count, GLsizei primcount) {
  if (primcount == 1) {
    glDrawArrays(mode, first, count);
  } else {
#ifdef GL_VERSION_3_0
    glDrawArraysInstanced(mode, first, count, primcount);
#else
    glDrawArraysInstancedARB(mode, first, count, primcount);
#endif
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
