#ifndef CUBOID_H
#define CUBOID_H

#include "geometry.h"
#include "glad/glad.h"
#include "glm.h"

namespace zxd {

class cuboid : public geometry_base {
protected:
  glm::vec3 m_half_diag;

public:
  cuboid(const glm::vec3& half_diag) : m_half_diag(half_diag) {}
  cuboid(GLfloat size = 1) : m_half_diag(0.5 * size, 0.5 * size, 0.5 * size) {}

  GLint build_vertex();
  // as i'm using strip and fan, normal should be vertex normal.
  GLint build_normal();
  GLint build_texcoord();

  void draw(GLuint primcount = 1);
};
}

#endif /* CUBOID_H */
