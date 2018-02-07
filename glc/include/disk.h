#ifndef DISK_H
#define DISK_H

#include "geometry.h"
#include "glm.h"

namespace zxd {

/*
 * just like glu disk
 */
class disk : public geometry {
protected:
  GLfloat m_inner;
  GLfloat m_outer;
  GLuint m_slice;
  GLuint m_loop;

public:
  disk() : m_inner(0.5), m_outer(1), m_slice(16), m_loop(8) {}
  disk(GLfloat inner, GLfloat outer, GLuint slice, GLuint loop)
      : m_inner(inner), m_outer(outer), m_slice(slice), m_loop(loop) {}

  void build_vertex();
  // as i'm using strip and fan, normal should be vertex normal.
  void build_normal();
  void build_texcoord();

  void draw(GLuint primcount = 1);

  GLfloat inner() const { return m_inner; }
  void inner(GLfloat v) { m_inner = v; }

  GLfloat outer() const { return m_outer; }
  void outer(GLfloat v) { m_outer = v; }

  GLuint slice() const { return m_slice; }
  void slice(GLuint v) { m_slice = v; }

  GLuint loop() const { return m_loop; }
  void loop(GLuint v) { m_loop = v; }
};
}

#endif /* DISK_H */
