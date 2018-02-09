#include "geometry.h"
#include "common.h"

namespace zxd {

//--------------------------------------------------------------------
void geometry_base::bind(GLint vertex, GLint normal /* = -1*/,
  GLint texcoord /* = -1*/, GLint tangent /* = -1*/) {
  bind_vertex_array_object();

  if (vertex != -1) {
    bind_vertex(vertex);
  }

  if (normal != -1) {
    bind_normal(normal);
  }

  if (texcoord != -1) {
    bind_texcoord(texcoord);
  }

  if (tangent != -1) {
    bind_tangent(tangent);
  }
}

//--------------------------------------------------------------------
void geometry_base::build_mesh(GLboolean normal /* = 1*/,
  GLboolean texcoord /* = 1*/, GLboolean tangent /* = -1*/) {
  build_vertex();
  buffer_vertex();

  if (normal) {
    build_normal();
    buffer_normal();
  }
  if (texcoord) {
    build_texcoord();
    buffer_texcoord();
  }
  if (tangent) {
    build_tangent();
    buffer_tangent();
  }
}

//--------------------------------------------------------------------
void geometry_base::draw_arrays(
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
void geometry_base::bind_vertex_array_object() {
  if (m_vao == -1) {
    glGenVertexArrays(1, &m_vao);
  }
  glBindVertexArray(m_vao);
}
}
