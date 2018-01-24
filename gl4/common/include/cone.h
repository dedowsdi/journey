#ifndef CONE_H
#define CONE_H

#include "geometry.h"
#include "glm.h"

namespace zxd {

/*
 * build sphere with two triangle fans and a bunch of triangle strips
 */
class Cone : public Geometry {
protected:
  GLfloat mRadius;
  GLfloat mHeight;
  GLuint mSlice;  // longitiude
  GLuint mStack;  // latitude

public:
  Cone() : mRadius(1), mHeight(1), mSlice(16), mStack(16) {}
  Cone(GLfloat radius, GLfloat height, GLuint slice, GLuint stack)
      : mRadius(radius), mHeight(height), mSlice(slice), mStack(stack) {}

  void buildVertex();
  // as i'm using strip and fan, normal should be vertex normal.
  void buildNormal();
  void buildTexcoord();

  void draw(GLuint primcount = 1);

  inline GLfloat getRadius() const { return mRadius; }
  inline void setRadius(GLfloat v) { mRadius = v; }

  inline GLuint getSlice() const { return mSlice; }
  inline void setSlice(GLuint v) { mSlice = v; }

  inline GLuint getStack() const { return mStack; }
  inline void setStack(GLuint v) { mStack = v; }

  inline GLfloat getHeight() const { return mHeight; }
  inline void setHeight(GLfloat v) { mHeight = v; }
};
}

#endif /* CONE_H */
