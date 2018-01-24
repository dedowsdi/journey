#include "cylinder.h"
#include "common.h"

namespace zxd {

//--------------------------------------------------------------------
void Cylinder::buildVertex(GLuint location) {
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

  // create stack from bottom to top
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

  bindVertexArrayObject();

  glGenBuffers(1, &mVertexBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
  glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(glm::vec3),
    value_ptr(mVertices[0]), GL_STATIC_DRAW);

  glVertexAttribPointer(location, 3, GL_FLOAT, 0, 0, BUFFER_OFFSET(0));
  glEnableVertexAttribArray(location);
}

//--------------------------------------------------------------------
void Cylinder::buildNormal(GLuint location) {
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

  bindVertexArrayObject();

  glGenBuffers(1, &mNormalBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, mNormalBuffer);
  glBufferData(GL_ARRAY_BUFFER, mNormals.size() * sizeof(glm::vec3),
    value_ptr(mNormals[0]), GL_STATIC_DRAW);

  glEnableVertexAttribArray(location);
  glVertexAttribPointer(location, 3, GL_FLOAT, 0, 0, BUFFER_OFFSET(0));
}

//--------------------------------------------------------------------
void Cylinder::buildTexcoord(GLuint location) {
  // t ranges from 0.0 at 0 to 1.0 at z = height, and s ranges from 0.0 at the
  // +y axis, to 0.25 at the +x axis, to 0.5 at the \-y axis, to 0.75 at the \-x
  // axis, and back to 1.0 at the +y axis

  mTexcoords.clear();
  mTexcoords.reserve(mVertices.size());

  float twoPi = 2 * glm::pi<GLfloat>();
  for (int i = 0; i < mVertices.size(); ++i) {
    GLfloat gamma = std::atan2(-mVertices[i].x, mVertices[i].y);
    if (gamma < 0) gamma += 2 * twoPi;

    float s = gamma / twoPi;
    float t = mVertices[i].z / mHeight;
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
