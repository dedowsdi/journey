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

class quad : public geometry2323 {
public:
  quad(const glm::vec3& half_diag) {}
  quad(GLfloat size = 1) {}

  void build_vertex();
  void build_normal();
  void build_texcoord();
  void build_tangent();

  void draw(GLuint primcount = 1);
};
}

#endif /* QUAD_H */
