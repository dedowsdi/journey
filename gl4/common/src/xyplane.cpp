#include "xyplane.h"

namespace zxd {

//--------------------------------------------------------------------
void xyplane::build_vertex() {
  m_vertices.clear();
  m_vertices.reserve((m_slice + 1) * (m_slice + 1));

  // build plane as triangle strip
  GLfloat xstep = m_width / m_slice;
  GLfloat ystep = m_height / m_slice;

  GLfloat left = -m_width / 2;
  GLfloat bottom = -m_height / 2;

  // build triangle strip as:
  //  0 2
  //  1 3
  for (int i = 0; i < m_slice; ++i) {  // row

    GLfloat y1 = bottom + ystep * i;
    GLfloat y0 = y1 + ystep;

    for (int j = 0; j <= m_slice; ++j) {  // col
      GLfloat x = left + xstep * j;

      vec3 v0(x, y0, 0);
      vec3 v1(x, y1, 0);
      m_vertices.push_back(v0);
      m_vertices.push_back(v1);
    }
  }
}

//--------------------------------------------------------------------
void xyplane::build_normal() {
  m_normals.clear();
  m_normals.reserve(m_vertices.size());

  for (int i = 0; i < m_vertices.size(); ++i) {
    m_normals.push_back(vec3(0, 0, 1));
  }

  assert(m_normals.size() == m_vertices.size());
}

//--------------------------------------------------------------------
void xyplane::build_texcoord() {
  m_texcoords.clear();
  m_texcoords.reserve(m_vertices.size());

  for (int i = 0; i < m_slice; ++i) {  // row

    GLfloat t0 = static_cast<GLfloat>(i) / m_slice;
    GLfloat t1 = static_cast<GLfloat>(i + 1) / m_slice;

    for (int j = 0; j <= m_slice; ++j) {  // col
      GLfloat s = static_cast<GLfloat>(j) / m_slice;

      vec2 tex0(s, t0);
      vec2 tex1(s, t1);
      m_texcoords.push_back(tex0);
      m_texcoords.push_back(tex1);
    }
  }
  assert(m_texcoords.size() == m_vertices.size());
}

//--------------------------------------------------------------------
void xyplane::draw(GLuint primcount /* = 1*/) {
  GLuint rowSize = (m_slice + 1) * 2;
  GLuint next = 0;
  
  bind_vertex_array_object();

  if (primcount == 1) {
    for (int i = 0; i < m_slice; ++i, next += rowSize) {
      glDrawArrays(GL_TRIANGLE_STRIP, next, rowSize);
    }
  } else {
    for (int i = 0; i < m_slice; ++i, next += rowSize) {
      glDrawArraysInstanced(GL_TRIANGLE_STRIP, next, rowSize, primcount);
    }
  }
}
}
