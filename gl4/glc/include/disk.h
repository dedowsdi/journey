#ifndef GL_GLC_DISK_H
#define GL_GLC_DISK_H

#include "geometry.h"

namespace zxd
{

// just like glu disk
class disk : public geometry_base
{

public:
  disk(GLfloat inner = 0.5f, GLfloat outer = 1.0f, GLuint slice = 8,
    GLuint loop = 8, GLfloat start = 0, GLfloat sweep = f2pi);

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

private:

  void build_vertex() override;
  void build_normal() override;
  void build_texcoord() override;

  GLfloat m_inner;
  GLfloat m_outer;
  GLfloat m_start; // in radian
  GLfloat m_sweep;
  GLuint m_slice;
  GLuint m_loop;

};
}

#endif /* GL_GLC_DISK_H */
