#ifndef SPHERE_H
#define SPHERE_H

#include "geometry.h"
#include "glm.h"

namespace zxd {

typedef std::vector<glm::vec3> Vec3Vector;
typedef std::vector<glm::vec2> Vec2Vector;

/*
 * build sphere with two triangle fans and a bunch of triangle strips
 */
class Sphere : public Geometry {
protected:
  GLfloat mRadius;
  GLuint mSlice;  // longitiude
  GLuint mStack;  // latitude

  Vec3Vector mVertices;
  Vec3Vector mNormals;
  Vec2Vector mTexcoords;

public:
  Sphere() : mRadius(1), mSlice(16), mStack(16) {}
  Sphere(GLfloat radius, GLuint slice, GLuint stack)
      : mRadius(radius), mSlice(slice), mStack(stack) {}

  void buildVertex(GLuint location);
  // as i'm using strip and fan, normal should be vertex normal.
  void buildNormal(GLuint location);
  void buildTexcoord(GLuint location);

  void draw(GLuint primcount = 1);

  inline GLfloat getRadius() const { return mRadius; }
  inline void setRadius(GLfloat v) { mRadius = v; }

  inline GLuint getSlice() const { return mSlice; }
  inline void setSlice(GLuint v) { mSlice = v; }

  inline GLuint getStack() const { return mStack; }
  inline void setStack(GLuint v) { mStack = v; }
};
}

#endif /* SPHERE_H */
