#ifndef GL_GLC_DISK_H
#define GL_GLC_DISK_H

#include "geometry.h"

namespace zxd {

/*
 * just like glu disk
 */
class disk : public geometry_base {
protected:
  GLfloat m_inner;
  GLfloat m_outer;
  GLfloat m_start; // in radian
  GLfloat m_sweep;
  GLuint m_slice;
  GLuint m_loop;

public:
  disk()
      : m_inner(0.5),
        m_outer(1),
        m_slice(16),
        m_loop(8),
        m_start(0),
        m_sweep(f2pi) {}

  disk(GLfloat inner, GLfloat outer, GLuint slice, GLuint loop)
      : m_inner(inner),
        m_outer(outer),
        m_slice(slice),
        m_loop(loop),
        m_start(0),
        m_sweep(f2pi) {}

  void build_vertex();
  // as i'm using strip and fan, normal should be vertex normal.
  void build_normal();
  void build_texcoord();

  GLfloat inner() const { return m_inner; }
  void inner(GLfloat v) { m_inner = v; }

  GLfloat outer() const { return m_outer; }
  void outer(GLfloat v) { m_outer = v; }

  GLuint slice() const { return m_slice; }
  void slice(GLuint v) { m_slice = v; }

  GLuint loop() const { return m_loop; }
  void loop(GLuint v) { m_loop = v; }

  GLfloat start() const { return m_start; }
  void start(GLfloat v) { m_start = v; }

  GLfloat sweep() const { return m_sweep; }
  void sweep(GLfloat v) { m_sweep = v; }
};
}

#endif /* GL_GLC_DISK_H */
