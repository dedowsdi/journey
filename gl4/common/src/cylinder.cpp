#include "cylinder.h"
#include "common.h"

namespace zxd {

//--------------------------------------------------------------------
void Cylinder::buildVertex() {
  GLfloat thetaStep = 2 * glm::pi<GLfloat>() / mSlice;  // azimuthal angle step
  GLfloat heightStep = mHeight / mStack;

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

  // create stack from bottom to top, different from sphere
  // build triangle strip as
  //    0 2
  //    1 3
  for (int i = 0; i < mStack; ++i) {
    for (int j = 0; j <= mSlice; j++) {
      // loop last stack in reverse order
      GLfloat theta = thetaStep * j;
      GLfloat cosTheta = std::cos(theta);
      GLfloat sinTheta = std::sin(theta);

      vec3 v1(mRadius * cosTheta, mRadius * sinTheta, heightStep * i);
      vec3 v0 = v1;
      v0.z += heightStep;

      mVertices.push_back(v0);
      mVertices.push_back(v1);
    }
  }

  // add top fan
  mVertices.push_back(vec3(0, 0, mHeight));
  for (int i = 0; i <= mSlice; ++i) {
    GLfloat theta = thetaStep * i;  // apply - for back face
    mVertices.push_back(
      vec3(mRadius * glm::cos(theta), mRadius * glm::sin(theta), mHeight));
  }
}

//--------------------------------------------------------------------
void Cylinder::buildNormal() {
  mNormals.clear();
  mNormals.reserve(mVertices.size());

  GLuint numVertBottom = mSlice + 2;

  // normals for bottom
  for (int i = 0; i < numVertBottom; ++i) {
    mNormals.push_back(vec3(0, 0, -1));
  }

  // normals for 1st stack
  for (int i = 0; i <= mSlice; ++i) {
    vec3 vertex = mVertices[numVertBottom + i * 2];
    vertex.z = 0;
    vec3 normal = glm::normalize(vertex);
    mNormals.push_back(normal);
    mNormals.push_back(normal);
  }

  // normals for other stacks
  for (int i = 1; i < mStack; ++i) {
    // no reallocate will happen here
    mNormals.insert(mNormals.end(), mNormals.begin() + numVertBottom,
      mNormals.begin() + numVertBottom + (mSlice + 1) * 2);
  }

  // normals for bottom
  for (int i = 0; i < numVertBottom; ++i) {
    mNormals.push_back(vec3(0, 0, 1));
  }
  assert(mNormals.size() == mVertices.size());
}

//--------------------------------------------------------------------
void Cylinder::buildTexcoord() {
  mTexcoords.clear();
  mTexcoords.reserve(mVertices.size());

  // bottom texcoords
  mTexcoords.push_back(glm::vec2(0, 0));
  for (int i = 0; i <= mSlice; ++i) {
    GLfloat s = 1 - static_cast<GLfloat>(i) / mSlice;
    mTexcoords.push_back(glm::vec2(s, 0));
  }

  for (int i = 0; i < mStack; ++i) {
    GLfloat t0 = static_cast<GLfloat>(i+1) / mStack;
    GLfloat t1 = static_cast<GLfloat>(i) / mStack;

    for (int j = 0; j <= mSlice; j++) {
      // loop last stack in reverse order
      GLfloat s = static_cast<GLfloat>(j) / mSlice;

      mTexcoords.push_back(glm::vec2(s, t0));
      mTexcoords.push_back(glm::vec2(s, t1));
    }
  }

  // top tex coords
  mTexcoords.push_back(glm::vec2(0, 1));
  for (int i = 0; i <= mSlice; ++i) {
    GLfloat s = static_cast<GLfloat>(i) / mSlice;
    mTexcoords.push_back(glm::vec2(s, 1));
  }

  assert(mTexcoords.size() == mVertices.size());
}

//--------------------------------------------------------------------
void Cylinder::draw(GLuint primcount /* = 1*/) {
  GLuint numVertBottom = mSlice + 2;  // pole + slice + 1
  GLuint numVertCenter = (mSlice + 1) * 2;

  bindVertexArrayObject();

  if (primcount == 1) {
    glDrawArrays(GL_TRIANGLE_FAN, 0, numVertBottom);

    GLuint next = numVertBottom;
    for (int i = 0; i < mStack; ++i, next += numVertCenter) {
      glDrawArrays(GL_TRIANGLE_STRIP, next, numVertCenter);
    }
    glDrawArrays(GL_TRIANGLE_FAN, next, numVertBottom);
  } else {
    glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, numVertBottom, primcount);

    GLuint next = numVertBottom;
    for (int i = 0; i < mStack - 2; ++i, next += numVertCenter) {
      glDrawArraysInstanced(GL_TRIANGLE_STRIP, next, numVertCenter, primcount);
    }
    glDrawArraysInstanced(GL_TRIANGLE_FAN, next, numVertBottom, primcount);
  }
}
}
