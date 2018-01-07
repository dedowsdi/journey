#ifndef GEOMETRY_H
#define GEOMETRY_H
#include "glad/glad.h"

namespace zxd {
class Geometry {
protected:
  GLuint mVao;

  GLuint mVertexBuffer;
  GLuint mNormalBuffer;
  GLuint mTexcoordBuffer;

public:
  Geometry() : mVao(-1) {}

  GLuint getVao() const { return mVao; }

  virtual void draw(GLuint primcount = 1) = 0;

protected:
  void bindVertexArrayObject();
};
}

#endif /* GEOMETRY_H */
