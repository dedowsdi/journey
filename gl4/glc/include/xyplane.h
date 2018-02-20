#ifndef XYPLANE_H
#define XYPLANE_H

#include "geometry.h"
#include "glm.h"

namespace zxd {

class xyplane : public geometry_base {
protected:
  GLuint m_slice;
  GLfloat m_width;
  GLfloat m_height;
  GLfloat m_left;
  GLfloat m_bottom;

public:
  xyplane()
      : m_slice(1), m_width(1), m_height(1), m_left(-0.5), m_bottom(-0.5) {}
  xyplane(GLfloat width, GLfloat height, GLuint slice)
      : m_width(width),
        m_height(height),
        m_slice(slice),
        m_left(-width * 0.5),
        m_bottom(-height * 0.5) {}

  xyplane(GLfloat x0, GLfloat y0, GLfloat x1, GLfloat y1)
      : m_width(x1 - x0),
        m_height(y1 - y0),
        m_slice(1),
        m_left(x0),
        m_bottom(y0) {}

  GLint build_vertex();
  // as i'm using strip and fan, normal should be vertex normal.
  GLint build_normal();
  GLint build_texcoord();

  void draw(GLuint primcount = 1);

  GLuint slice() const { return m_slice; }
  void slice(GLuint v) { m_slice = v; }

  GLfloat width() const { return m_width; }
  void width(GLfloat v) { m_width = v; }

  GLfloat height() const { return m_height; }
  void height(GLfloat v) { m_height = v; }

  GLfloat left() const { return m_left; }
  void left(GLfloat v){ m_left = v; }

  GLfloat bottom() const { return m_bottom; }
  void bottom(GLfloat v){ m_bottom = v; }

};
}

#endif /* XYPLANE_H */
