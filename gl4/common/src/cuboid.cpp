#include "cuboid.h"
#include "common.h"

namespace zxd {

//--------------------------------------------------------------------
void Cuboid::buildVertex() {
  mVertices.clear();
  mVertices.reserve(24);

  GLfloat x = mHalfDiag.x;
  GLfloat y = mHalfDiag.y;
  GLfloat z = mHalfDiag.z;
  GLfloat x2 = x * 2;
  GLfloat y2 = y * 2;
  GLfloat z2 = z * 2;

  // make sure that if  rotate around z, origin of every face is at low left
  // corner.
  glm::vec3 origins[6] = {
    vec3(-x, -y, -z),  // front
    vec3(x, y, -z),    // back
    vec3(x, -y, -z),   // right
    vec3(-x, y, -z),   // left
    vec3(-x, -y, z),   // top
    vec3(-x, y, -z)    // bottom
  };

  glm::vec3 stepXs[6] = {
    vec3(x2, 0, 0), vec3(-x2, 0, 0),  // front, back
    vec3(0, y2, 0), vec3(0, -y2, 0),  // right, left
    vec3(x2, 0, 0), vec3(x2, 0, 0)    // top, bottom
  };

  glm::vec3 stepYs[6] = {
    vec3(0, 0, z2), vec3(0, 0, z2),  // front, back
    vec3(0, 0, z2), vec3(0, 0, z2),  // right, left
    vec3(0, y2, 0), vec3(0, -y2, 0)  // top, bottom
  };

  for (int i = 0; i < 6; ++i) {
    const vec3& origin = origins[i];
    const vec3& stepX = stepXs[i];
    const vec3& stepY = stepYs[i];

    mVertices.push_back(origin);
    mVertices.push_back(origin + stepX);
    mVertices.push_back(origin + stepX + stepY);

    mVertices.push_back(origin);
    mVertices.push_back(origin + stepX + stepY);
    mVertices.push_back(origin + stepY);
  }
}

//--------------------------------------------------------------------
void Cuboid::buildNormal() {
  mNormals.clear();
  mNormals.reserve(mVertices.size());
  for (int i = 0; i < 6; ++i) mNormals.push_back(glm::vec3(0, -1, 0));
  for (int i = 0; i < 6; ++i) mNormals.push_back(glm::vec3(0, 1, 0));
  for (int i = 0; i < 6; ++i) mNormals.push_back(glm::vec3(1, 0, 0));
  for (int i = 0; i < 6; ++i) mNormals.push_back(glm::vec3(-1, 0, 0));
  for (int i = 0; i < 6; ++i) mNormals.push_back(glm::vec3(0, 0, 1));
  for (int i = 0; i < 6; ++i) mNormals.push_back(glm::vec3(0, 0, -1));
}

//--------------------------------------------------------------------
void Cuboid::buildTexcoord() {
  mTexcoords.clear();
  mTexcoords.reserve(mVertices.size());

  for (int i = 0; i < 6; ++i) {
    mTexcoords.push_back(glm::vec2(0, 0));
    mTexcoords.push_back(glm::vec2(1, 0));
    mTexcoords.push_back(glm::vec2(1, 1));
    mTexcoords.push_back(glm::vec2(0, 0));
    mTexcoords.push_back(glm::vec2(1, 1));
    mTexcoords.push_back(glm::vec2(0, 1));
  }
}

//--------------------------------------------------------------------
void Cuboid::draw(GLuint primcount /* = 1*/) {
  bindVertexArrayObject();
  if (primcount == 1) {
    glDrawArrays(GL_TRIANGLES, 0, mVertices.size());
    // glDrawArrays(GL_TRIANGLES, 0, 12);
  } else {
    glDrawArraysInstanced(GL_TRIANGLES, 0, mVertices.size(), primcount);
  }
}
}
