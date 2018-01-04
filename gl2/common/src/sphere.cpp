#include "sphere.h"
#include "common.h"

namespace zxd {

//--------------------------------------------------------------------
void Sphere::buildVertex(GLuint location) {
  GLfloat phiStep = glm::pi<GLfloat>() / mStack;
  GLfloat thetaStep = 2 * glm::pi<GLfloat>() / mSlice;

  GLuint numVertPole = mSlice + 1;
  GLuint numVertCenter = mSlice * 2;

  mVertices.reserve(numVertPole * 2 + numVertCenter * (mStack - 2));

  // create sphere stack by stack
  for (int i = 0; i < mStack; ++i) {
    GLfloat phi0 = i * phiStep;
    GLfloat phi1 = phi0 + phiStep;

    GLfloat sinPhi0 = std::sin(phi0);
    GLfloat sinPhi1 = std::sin(phi1);
    GLfloat cosPhi0 = std::cos(phi0);
    GLfloat cosPhi1 = std::cos(phi1);

    GLfloat rTimesSinPhi0 = mRadius * sinPhi0;
    GLfloat rTimesSinPhi1 = mRadius * sinPhi1;
    GLfloat rTimesCosPhi0 = mRadius * cosPhi0;
    GLfloat rTimesCosPhi1 = mRadius * cosPhi1;

    // add pole
    if (i == 0) mVertices.push_back(glm::vec3(0, 0, mRadius));
    if (i == mStack - 1) mVertices.push_back(glm::vec3(0, 0, -mRadius));

    for (int j = 0; j <= mSlice; j++) {
      // loop last stack in reverse order
      GLfloat theta = thetaStep * (i == mStack - 1 ? -j : j);
      GLfloat cosTheta = std::cos(theta);
      GLfloat sinTheta = std::sin(theta);

      // x = rho * sin(phi) * cos(theta)
      // x = rho * sin(phi) * sin(theta)
      // z = rho * cos(phi)
      if (i != 0)
        mVertices.push_back(vec3(
          rTimesSinPhi0 * cosTheta, rTimesSinPhi0 * sinTheta, rTimesCosPhi0));

      if (i != mStack - 1)
        mVertices.push_back(vec3(
          rTimesSinPhi1 * cosTheta, rTimesSinPhi1 * sinTheta, rTimesCosPhi1));
    }

  }

  bindVertexArrayObject();

  glGenBuffers(1, &mVertexBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
  glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(glm::vec3),
    value_ptr(mVertices[0]), GL_STATIC_DRAW);

  glVertexAttribPointer(location, 3, GL_FLOAT, 0, 0, BUFFER_OFFSET(0));
  glEnableVertexAttribArray(location);
}

//--------------------------------------------------------------------
void Sphere::buildNormal(GLuint location) {
  mNormals.reserve(mVertices.size());
  for (int i = 0; i < mVertices.size(); ++i) {
    mNormals.push_back(glm::normalize(mVertices[i]));
  }

  bindVertexArrayObject();

  glGenBuffers(1, &mNormalBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, mNormalBuffer);
  glBufferData(GL_ARRAY_BUFFER, mNormals.size() * sizeof(glm::vec3),
    value_ptr(mNormals[0]), GL_STATIC_DRAW);

  glEnableVertexAttribArray(location);
  glVertexAttribPointer(location, 3, GL_FLOAT, 0, 0, BUFFER_OFFSET(0));
}

//--------------------------------------------------------------------
void Sphere::buildTexcoord(GLuint location) {
  float twoPi = 2 * glm::pi<GLfloat>();
  for (int i = 0; i < mVertices.size(); ++i) {
    GLfloat cosPhi = mVertices[i].z / mRadius;
    GLfloat phi = std::acos(cosPhi);
    GLfloat sinPhi = std::sin(phi);
    GLfloat cosTheta = mVertices[i].x / (mRadius * sinPhi);
    GLfloat sinTheta = mVertices[i].y / (mRadius * sinPhi);

    GLfloat gamma = std::atan2(-cosTheta, sinTheta);
    if (gamma < 0) gamma += 2 * twoPi;

    float s = gamma / twoPi;
    float t = phi / twoPi;
    mTexcoords.push_back(glm::vec2(s, t));
  }

  bindVertexArrayObject();

  glGenBuffers(1, &mTexcoordBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, mTexcoordBuffer);
  glBufferData(GL_ARRAY_BUFFER, mTexcoords.size() * sizeof(glm::vec2),
    value_ptr(mTexcoords[0]), GL_STATIC_DRAW);

  glEnableVertexAttribArray(location);
  glVertexAttribPointer(location, 2, GL_FLOAT, 0, 0, BUFFER_OFFSET(0));
}

//--------------------------------------------------------------------
void Sphere::draw(GLuint primcount /* = 1*/) {
  GLuint numVertPole = mSlice + 2;  // pole + slice + 1
  GLuint numVertCenter = (mSlice + 1) * 2;

  bindVertexArrayObject();

  if (primcount == 1) {
    glDrawArrays(GL_TRIANGLE_FAN, 0, numVertPole);

    GLuint next = numVertPole;
    for (int i = 0; i < mStack - 2; ++i, next += numVertCenter) {
      glDrawArrays(GL_QUAD_STRIP, next, numVertCenter);
    }
    glDrawArrays(GL_TRIANGLE_FAN, next, numVertPole);
  } else {
    glDrawArraysInstancedARB(GL_TRIANGLE_FAN, 0, numVertPole, primcount);

    GLuint next = numVertPole;
    for (int i = 0; i < mStack - 2; ++i, next += numVertCenter) {
      glDrawArraysInstancedARB(GL_QUAD_STRIP, next, numVertCenter, primcount);
    }
    glDrawArraysInstancedARB(GL_TRIANGLE_FAN, next, numVertPole, primcount);
  }
}

//--------------------------------------------------------------------
void Sphere::bindVertexArrayObject() {
  if (mVao == -1) {
    glGenVertexArrays(1, &mVao);
  }
  glBindVertexArray(mVao);
}
}
