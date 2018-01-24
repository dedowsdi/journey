#include "torus.h"
#include "common.h"

namespace zxd {

//--------------------------------------------------------------------
void Torus::buildVertex() {
  GLuint ringSize = (mSides + 1) * 2;
  GLfloat pi2 = glm::pi<GLfloat>() * 2;
  GLfloat thetaStep = pi2 / mRings;
  GLfloat phiStep = pi2 / mSides;

  mVertices.clear();
  mVertices.reserve(ringSize * mRings);

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
    }
  }
}

//--------------------------------------------------------------------
void Torus::buildNormal() {
  mNormals.clear();
  mNormals.reserve(mVertices.capacity());

  GLfloat pi2 = glm::pi<GLfloat>() * 2;
  GLfloat thetaStep = pi2 / mRings;
  GLfloat phiStep = pi2 / mSides;

  for (int i = 0; i < mRings; ++i) {
    GLfloat theta0 = thetaStep * i;
    GLfloat theta1 = theta0 + thetaStep;
    GLfloat cosTheta0 = glm::cos(theta0);
    GLfloat cosTheta1 = glm::cos(theta1);
    GLfloat sinTheta0 = glm::sin(theta0);
    GLfloat sinTheta1 = glm::sin(theta1);

    for (int j = 0; j <= mSides; ++j) {
      GLfloat phi = phiStep * j;

      GLfloat r = mOuterRadius + mInnerRadius * glm::cos(phi);
      vec3 v0(cosTheta0 * r, sinTheta0 * r, mInnerRadius * glm::sin(phi));
      vec3 v1(cosTheta1 * r, sinTheta1 * r, mInnerRadius * glm::sin(phi));

      // normal is the same as vertex - circle center
      vec3 n0 =
        v0 - vec3(cosTheta0 * mOuterRadius, sinTheta0 * mOuterRadius, 0);
      vec3 n1 =
        v1 - vec3(cosTheta1 * mOuterRadius, sinTheta1 * mOuterRadius, 0);

      mNormals.push_back(glm::normalize(n0));
      mNormals.push_back(glm::normalize(n1));
    }
  }

  assert(mNormals.size() == mVertices.size());
}

//--------------------------------------------------------------------
void Torus::buildTexcoord() {
  GLfloat pi2 = glm::pi<GLfloat>() * 2;
  GLfloat thetaStep = pi2 / mRings;
  GLfloat phiStep = pi2 / mSides;

  mTexcoords.clear();
  mTexcoords.reserve(mVertices.capacity());

  // Build torus ring by ring(triangle strip), use theta along +z to rotate
  // ring, use phi along -y to rotate vertex in ring.
  // see image in http://paulbourke.net/geometry/torus/
  for (int i = 0; i < mRings; ++i) {
    GLfloat theta0 = thetaStep * i;
    GLfloat theta1 = theta0 + thetaStep;

    for (int j = 0; j <= mSides; ++j) {
      GLfloat phi = phiStep * j;

      vec2 t0(theta0 / pi2, phi / pi2);
      vec2 t1(theta1 / pi2, phi / pi2);

      mTexcoords.push_back(t0);
      mTexcoords.push_back(t1);
    }
  }

  assert(mTexcoords.size() == mVertices.size());
}

//--------------------------------------------------------------------
void Torus::draw(GLuint primcount /* = 1*/) {
  bindVertexArrayObject();

  GLuint ringSize = (mSides + 1) * 2;

  if (primcount == 1) {
    for (int i = 0; i < mRings; ++i) {
      glDrawArrays(GL_TRIANGLE_STRIP, ringSize * i, ringSize);
    }
  } else {
    for (int i = 0; i < mRings; ++i) {
      glDrawArraysInstanced(
        GL_TRIANGLE_STRIP, ringSize * i, ringSize, primcount);
    }
  }
}
}
