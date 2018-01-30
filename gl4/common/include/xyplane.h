#ifndef XYPLANE_H
#define XYPLANE_H

#include "geometry.h"
#include "glm.h"

namespace zxd {

class xyplane : public geometry {
protected:
  GLuint m_slice;
  GLfloat m_width;
  GLfloat m_height;

public:
  xyplane() : m_slice(1), m_width(1), m_height(1) {}
  xyplane(GLfloat width, GLfloat height, GLuint slice)
      : m_width(width), m_height(height), m_slice(slice) {}

  void build_vertex();
  // as i'm using strip and fan, normal should be vertex normal.
  void build_normal();
  void build_texcoord();

  void draw(GLuint primcount = 1);

  GLuint slice() const { return m_slice; }
  void slice(GLuint v) { m_slice = v; }

  GLfloat width() const { return m_width; }
  void width(GLfloat v) { m_width = v; }

  GLfloat getHeight() const { return m_height; }
  void set_height(GLfloat v) { m_height = v; }
};
}

#endif /* XYPLANE_H */
