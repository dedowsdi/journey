#ifndef CYLINDER_H
#define CYLINDER_H

#include "geometry.h"
#include "glm.h"

namespace zxd {

/*
 * build cylinder with two triangle fans and a bunch of triangle strips
 */
class cylinder : public geometry3323 {
protected:
  GLfloat m_base;
  GLfloat m_top;
  GLfloat m_height;
  GLuint m_slice;  // longitiude
  GLuint m_stack;  // latitude

public:
  cylinder() : m_base(1), m_top(1), m_height(1), m_slice(16), m_stack(16) {}
  cylinder(
    GLfloat base, GLfloat top, GLfloat height, GLuint slice, GLuint stack)
      : m_base(base),
        m_top(top),
        m_height(height),
        m_slice(slice),
        m_stack(stack) {}

  void build_vertex();
  // as i'm using strip and fan, normal should be vertex normal.
  void build_normal();
  void build_texcoord();

  void draw(GLuint primcount = 1);

  GLuint slice() const { return m_slice; }
  void slice(GLuint v) { m_slice = v; }

  GLuint stack() const { return m_stack; }
  void stack(GLuint v) { m_stack = v; }

  GLfloat height() const { return m_height; }
  void height(GLfloat v) { m_height = v; }

  GLfloat base() const { return m_base; }
  void base(GLfloat v) { m_base = v; }

  GLfloat top() const { return m_top; }
  void top(GLfloat v) { m_top = v; }
};
}

#endif /* CYLINDER_H */
