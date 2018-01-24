#ifndef CUBOID_H
#define CUBOID_H

#include "geometry.h"
#include "glad/glad.h"
#include "glm.h"

namespace zxd {

typedef std::vector<glm::vec3> Vec3Vector;
typedef std::vector<glm::vec2> Vec2Vector;

class Cuboid : public Geometry {
protected:
  GLuint mVao;

  GLuint mVertexBuffer;
  GLuint mNormalBuffer;
  GLuint mTexcoordBuffer;

  glm::vec3 mHalfDiag;

public:
  Cuboid(const glm::vec3& halfDiag) : mHalfDiag(halfDiag) {}
  Cuboid(GLfloat size = 1) : mHalfDiag(0.5 * size, 0.5 * size, 0.5 * size) {}

  void buildVertex();
  // as i'm using strip and fan, normal should be vertex normal.
  void buildNormal();
  void buildTexcoord();

  void draw(GLuint primcount = 1);
};
}

#endif /* CUBOID_H */
