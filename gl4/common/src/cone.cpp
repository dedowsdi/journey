#include "cone.h"
#include "common.h"

namespace zxd {

//--------------------------------------------------------------------
void Cone::buildVertex() {
  GLfloat thetaStep = f2pi / mSlice;  // azimuthal angle step
  GLfloat heightStep = mHeight / mStack;
  GLfloat radiusStep = mRadius / mStack;

  GLuint numVertPole = mSlice + 1;
  GLuint numVertCenter = (mSlice + 1) * 2;

  mVertices.reserve(numVertPole * 2 + numVertCenter * (mStack - 1));

  // add bottom fan
  mVertices.push_back(vec3(0.0));
  for (int i = 0; i <= mSlice; ++i) {
    GLfloat theta = -thetaStep * i;  // apply - for back face
    mVertices.push_back(
      vec3(mRadius * glm::cos(theta), mRadius * glm::sin(theta), 0));
  }

  // create stack from bottom to top, different sphere.
  // build triangle strip as
  //    0 2
  //    1 3
  for (int i = 0; i < mStack; ++i) {
    GLfloat h0 = heightStep * i;
    GLfloat h1 = h0 + heightStep;
    GLfloat r0 = radiusStep * (mStack - i);
    GLfloat r1 = r0 - radiusStep;

    // add hat
    if (i == mStack - 1) mVertices.push_back(glm::vec3(0, 0, mHeight));

    for (int j = 0; j <= mSlice; j++) {
      // loop last stack in reverse order
      GLfloat theta = thetaStep * j;
      GLfloat cosTheta = std::cos(theta);
      GLfloat sinTheta = std::sin(theta);

      if (i != mStack - 1) {
        mVertices.push_back(vec3(r1 * cosTheta, r1 * sinTheta, h1));
      }

      mVertices.push_back(vec3(r0 * cosTheta, r0 * sinTheta, h0));
    }
  }
}

//--------------------------------------------------------------------
void Cone::buildNormal() {
  mNormals.clear();
  mNormals.reserve(mVertices.size());

  GLuint numVertBottom = mSlice + 2;

  // normals for bottom
  for (int i = 0; i < numVertBottom; ++i) {
    mNormals.push_back(vec3(0, 0, -1));
  }

  glm::vec3 apex(0, 0, mHeight);

  // normals for 1st stack
  for (int i = 0; i <= mSlice; ++i) {
    const vec3& vertex = mVertices[numVertBottom + i * 2];
    vec3 outer = glm::cross(vertex, apex);
    vec3 normal = glm::cross(apex - vertex, outer);
    mNormals.push_back(normal);
    mNormals.push_back(normal);
  }

  // normals for 1 - last 2 stacks
  for (int i = 1; i < mStack - 1; ++i) {
    // no reallocate will happen here
    mNormals.insert(mNormals.end(), mNormals.begin() + numVertBottom,
      mNormals.begin() + numVertBottom + (mSlice + 1) * 2);
  }

  // normals for last stacks
  mNormals.push_back(vec3(0, 0, 1));
  for (int i = 0; i <= mSlice; ++i) {
    mNormals.push_back(mNormals[numVertBottom + i * 2]);
  }
  assert(mNormals.size() == mVertices.size());
}

//--------------------------------------------------------------------
void Cone::buildTexcoord() {
  mTexcoords.clear();
  mTexcoords.reserve(mVertices.size());

  // bottom texcoords
  mTexcoords.push_back(glm::vec2(0, 0));
  for (int i = 0; i <= mSlice; ++i) {
    GLfloat s = 1 - static_cast<GLfloat>(i) / mSlice;
    mTexcoords.push_back(glm::vec2(s, 0));
  }

  for (int i = 0; i < mStack; ++i) {
    GLfloat t0 = static_cast<GLfloat>(i + 1) / mStack;
    GLfloat t1 = static_cast<GLfloat>(i) / mStack;

    // add pole
    if (i == mStack - 1) mTexcoords.push_back(glm::vec2(1, 0));

    for (int j = 0; j <= mSlice; j++) {
      // loop last stack in reverse order
      GLfloat s = static_cast<GLfloat>(j) / mSlice;

      mTexcoords.push_back(glm::vec2(s, t0));

      if (i != mStack - 1) mTexcoords.push_back(glm::vec2(s, t1));
    }
  }

  assert(mTexcoords.size() == mVertices.size());
}

//--------------------------------------------------------------------
void Cone::draw(GLuint primcount /* = 1*/) {
  GLuint numVertPole = mSlice + 2;  // pole + slice + 1
  GLuint numVertCenter = (mSlice + 1) * 2;

  bindVertexArrayObject();

  if (primcount == 1) {
    glDrawArrays(GL_TRIANGLE_FAN, 0, numVertPole);

    GLuint next = numVertPole;
    for (int i = 0; i < mStack - 1; ++i, next += numVertCenter) {
      glDrawArrays(GL_TRIANGLE_STRIP, next, numVertCenter);
    }
    glDrawArrays(GL_TRIANGLE_FAN, next, numVertPole);
  } else {
    glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, numVertPole, primcount);

    GLuint next = numVertPole;
    for (int i = 0; i < mStack - 2; ++i, next += numVertCenter) {
      glDrawArraysInstanced(GL_TRIANGLE_STRIP, next, numVertCenter, primcount);
    }
    glDrawArraysInstanced(GL_TRIANGLE_FAN, next, numVertPole, primcount);
  }
}
}
