#ifndef XYPLANE_H
#define XYPLANE_H

#include "geometry.h"
#include "glm.h"

namespace zxd {

class Xyplane : public Geometry {
protected:
  GLuint mSlice;
  GLfloat mWidth;
  GLfloat mHeight;

public:
  Xyplane() : mSlice(1), mWidth(1), mHeight(1) {}
  Xyplane(GLfloat width, GLfloat height, GLuint slice)
      : mWidth(width), mHeight(height), mSlice(slice) {}

  void buildVertex();
  // as i'm using strip and fan, normal should be vertex normal.
  void buildNormal();
  void buildTexcoord();

  void draw(GLuint primcount = 1);

  GLuint getSlice() const { return mSlice; }
  void setSlice(GLuint v) { mSlice = v; }

  GLfloat getWidth() const { return mWidth; }
  void setWidth(GLfloat v) { mWidth = v; }

  GLfloat getHeight() const { return mHeight; }
  void setHeight(GLfloat v) { mHeight = v; }
};
}

#endif /* XYPLANE_H */
