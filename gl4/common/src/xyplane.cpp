#include "xyplane.h"

namespace zxd {

//--------------------------------------------------------------------
void Xyplane::buildVertex() {
  mVertices.clear();
  mVertices.reserve((mSlice + 1) * (mSlice + 1));

  // build plane as triangle strip
  GLfloat xstep = mWidth / mSlice;
  GLfloat ystep = mHeight / mSlice;

  GLfloat left = -mWidth / 2;
  GLfloat bottom = -mHeight / 2;

  // build triangle strip as:
  //  0 2
  //  1 3
  for (int i = 0; i < mSlice; ++i) {  // row

    GLfloat y1 = bottom + ystep * i;
    GLfloat y0 = y1 + ystep;

    for (int j = 0; j <= mSlice; ++j) {  // col
      GLfloat x = left + xstep * j;

      vec3 v0(x, y0, 0);
      vec3 v1(x, y1, 0);
      mVertices.push_back(v0);
      mVertices.push_back(v1);
    }
  }
}

//--------------------------------------------------------------------
void Xyplane::buildNormal() {
  mNormals.clear();
  mNormals.reserve(mVertices.size());

  for (int i = 0; i < mVertices.size(); ++i) {
    mNormals.push_back(vec3(0, 0, 1));
  }

  assert(mNormals.size() == mVertices.size());
}

//--------------------------------------------------------------------
void Xyplane::buildTexcoord() {
  mTexcoords.clear();
  mTexcoords.reserve(mVertices.size());

  for (int i = 0; i < mSlice; ++i) {  // row

    GLfloat t0 = static_cast<GLfloat>(i) / mSlice;
    GLfloat t1 = static_cast<GLfloat>(i + 1) / mSlice;

    for (int j = 0; j <= mSlice; ++j) {  // col
      GLfloat s = static_cast<GLfloat>(j) / mSlice;

      vec2 tex0(s, t0);
      vec2 tex1(s, t1);
      mTexcoords.push_back(tex0);
      mTexcoords.push_back(tex1);
    }
  }
  assert(mTexcoords.size() == mVertices.size());
}

//--------------------------------------------------------------------
void Xyplane::draw(GLuint primcount /* = 1*/) {
  GLuint rowSize = (mSlice + 1) * 2;
  GLuint next = 0;
  
  bindVertexArrayObject();

  if (primcount == 1) {
    for (int i = 0; i < mSlice; ++i, next += rowSize) {
      glDrawArrays(GL_TRIANGLE_STRIP, next, rowSize);
    }
  } else {
    for (int i = 0; i < mSlice; ++i, next += rowSize) {
      glDrawArraysInstanced(GL_TRIANGLE_STRIP, next, rowSize, primcount);
    }
  }
}
}
