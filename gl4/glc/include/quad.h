#ifndef QUAD_H
#define QUAD_H
#include "program.h"
#include "glm.h"
#include <array>
#include "common.h"
#include "geometry.h"

namespace zxd {

// draw single 2d quad
void draw_quad(GLuint tex_index = 0);

class quad : public geometry_base {
protected:
  GLfloat m_width;
  GLfloat m_height;
public:
  quad(const glm::vec3& half_diag) {}
  quad(GLfloat size = 2) : m_width(size), m_height(size) {}

  GLint build_vertex();
  GLint build_normal();
  GLint build_texcoord();
  GLint build_tangent();

  void draw(GLuint primcount = 1);

  GLfloat width() const { return m_width; }
  void width(GLfloat v){ m_width = v; }

  GLfloat height() const { return m_height; }
  void height(GLfloat v){ m_height = v; }

};
}

#endif /* QUAD_H */
