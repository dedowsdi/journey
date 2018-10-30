#ifndef CUBOID_H
#define CUBOID_H

#include "geometry.h"
#include "glm.h"

namespace zxd {

class cuboid : public geometry_base {
protected:
  glm::vec3 m_half_diag;

public:
  cuboid(const glm::vec3& half_diag) : m_half_diag(half_diag) {}
  cuboid(GLfloat size = 1) : m_half_diag(0.5 * size, 0.5 * size, 0.5 * size) {}

  void build_vertex();
  // as i'm using strip and fan, normal should be vertex normal.
  void build_normal();
  void build_texcoord();

  const glm::vec3& half_diag() const { return m_half_diag; }
  void half_diag(const glm::vec3& v){ m_half_diag = v; }
};
}

#endif /* CUBOID_H */
