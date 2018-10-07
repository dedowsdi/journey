#ifndef CONE_H
#define CONE_H

#include "geometry.h"
#include "glm.h"

namespace zxd {

/*
 * build sphere with two triangle fans and a bunch of triangle strips
 */
class cone : public geometry_base {
protected:
  GLfloat m_radius;
  GLfloat m_height;
  GLuint m_slice;  // longitiude
  GLuint m_stack;  // latitude

public:
  cone() : m_radius(1), m_height(1), m_slice(16), m_stack(16) {}
  cone(GLfloat radius, GLfloat height, GLuint slice, GLuint stack)
      : m_radius(radius), m_height(height), m_slice(slice), m_stack(stack) {}

  void build_vertex();
  // as i'm using strip and fan, normal should be vertex normal.
  void build_normal();
  void build_texcoord();

  void draw(GLuint primcount = 1);

  GLfloat radius() const { return m_radius; }
  void radius(GLfloat v) { m_radius = v; }

  GLuint slice() const { return m_slice; }
  void slice(GLuint v) { m_slice = v; }

  GLuint stack() const { return m_stack; }
  void stack(GLuint v) { m_stack = v; }

  GLfloat height() const { return m_height; }
  void height(GLfloat v) { m_height = v; }
};
}

#endif /* CONE_H */
