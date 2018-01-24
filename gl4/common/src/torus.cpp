#include "torus.h"
#include "common.h"

namespace zxd {

//--------------------------------------------------------------------
void Torus::buildVertex(GLuint location) {
  GLuint ringSize = (mSides + 1) * 2;
  GLfloat pi2 = glm::pi<GLfloat>() * 2;
  GLfloat thetaStep = pi2 / mRings;
  GLfloat phiStep = pi2 / mSides;

  mVertices.reserve(ringSize * mRings);
  mNormals.reserve(mVertices.capacity());

  // Build torus ring by ring(triangle strip), use theta along +z to rotate
  // ring, use phi along -y to rotate vertex in ring.
  // see image in http://paulbourke.net/geometry/torus/
  for (int i = 0; i < mRings; ++i) {
    GLfloat theta0 = thetaStep * i;
    GLfloat theta1 = theta0 + thetaStep;
    GLfloat cosTheta0 = glm::cos(theta0);
    GLfloat cosTheta1 = glm::cos(theta1);
    GLfloat sinTheta0 = glm::sin(theta0);
    GLfloat sinTheta1 = glm::sin(theta1);

    for (int j = 0; j <= mSides; ++j) {
      GLfloat phi = phiStep * j;

      // The first circle resides on xz plane, projection of vertex of this
      // circle on xy plane is (0, 0, r), rotate it along -y, and you get the
      // xy part of final vertex
      GLfloat r = mOuterRadius + mInnerRadius * glm::cos(phi);
      vec3 v0(cosTheta0 * r, sinTheta0 * r, mInnerRadius * glm::sin(phi));
      vec3 v1(cosTheta1 * r, sinTheta1 * r, mInnerRadius * glm::sin(phi));

      mVertices.push_back(v0);
      mVertices.push_back(v1);

      // normal is the same as vertex - circle center
      vec3 n0 = v0 - vec3(cosTheta0 * mOuterRadius, sinTheta0 * mOuterRadius, 0);
      vec3 n1 = v1 - vec3(cosTheta1 * mOuterRadius, sinTheta1 * mOuterRadius, 0);

      mNormals.push_back(glm::normalize(n0));
      mNormals.push_back(glm::normalize(n1));

    }
  }

  //We can define a torus by the equations:

    //[Note: p = Pi = 3.141592. Define q as the angle from the X axis (0 <= q <= 2p) and f as the angle from the Z axis (0.0 <= f <= p).]

     

    //Then, with R as the inner radius and r as the outer radius:

      //X = (R + r cos (2 pv)) cos (2 pu)
      //Y = r sin (2 pv) 
      //Z = (R + r cos (2 pv)) sin (2 pu)

      //Then:

      //v = arccos (Y/R)/2p
      //u = [arccos ((X/(R + r*cos(2 pv))]2p

  bindVertexArrayObject();

  glGenBuffers(1, &mVertexBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
  glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(glm::vec3),
    value_ptr(mVertices[0]), GL_STATIC_DRAW);

  glVertexAttribPointer(location, 3, GL_FLOAT, 0, 0, BUFFER_OFFSET(0));
  glEnableVertexAttribArray(location);
}

//--------------------------------------------------------------------
void Torus::buildNormal(GLuint location) {

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
void Torus::buildTexcoord(GLuint location) {

  bindVertexArrayObject();

  glGenBuffers(1, &mTexcoordBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, mTexcoordBuffer);
  glBufferData(GL_ARRAY_BUFFER, mTexcoords.size() * sizeof(glm::vec2),
    value_ptr(mTexcoords[0]), GL_STATIC_DRAW);

  glEnableVertexAttribArray(location);
  glVertexAttribPointer(location, 2, GL_FLOAT, 0, 0, BUFFER_OFFSET(0));
}

//--------------------------------------------------------------------
void Torus::draw(GLuint primcount /* = 1*/) {

  bindVertexArrayObject();

  GLuint ringSize = (mSides + 1)  * 2;

  if (primcount == 1) {
    for (int i = 0; i < mRings; ++i) {
      glDrawArrays(GL_TRIANGLE_STRIP, ringSize * i, ringSize);
    }
  } else {
    for (int i = 0; i < mRings; ++i) {
      glDrawArraysInstanced(GL_TRIANGLE_STRIP, ringSize * i, ringSize, primcount);
    }
  }
}
}
