#ifndef GEOMETRY_H
#define GEOMETRY_H
#include "glad/glad.h"
#include "glm.h"

namespace zxd {
class Geometry {
protected:
  GLuint mVao;

  GLuint mVertexBuffer;
  GLuint mNormalBuffer;
  GLuint mTexcoordBuffer;

  Vec3Vector mVertices;
  Vec3Vector mNormals;
  Vec2Vector mTexcoords;

public:
  Geometry() : mVao(-1) {}

  GLuint getVao() const { return mVao; }

  void bind(GLint vertex, GLint normal = -1, GLint texcoord = -1);
  virtual void draw(GLuint primcount = 1) = 0;

  void buildMesh(GLboolean normal = 1, GLboolean texcoord = 1);
  virtual void buildVertex(){};
  // as i'm using strip and fan, normal should be vertex normal.
  virtual void buildNormal(){};
  virtual void buildTexcoord(){};

protected:
  void bindVertexArrayObject();
};
}

#endif /* GEOMETRY_H */
