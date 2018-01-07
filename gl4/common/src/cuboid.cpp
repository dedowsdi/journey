#include "cuboid.h"
#include "common.h"

namespace zxd {

//--------------------------------------------------------------------
void Cuboid::buildVertex(GLuint location) {
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
    vec3(0, y2, 0), vec3(0, -y2, 0)    // top, bottom
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

  bindVertexArrayObject();

  glGenBuffers(1, &mVertexBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
  glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(glm::vec3),
    value_ptr(mVertices[0]), GL_STATIC_DRAW);

  glVertexAttribPointer(location, 3, GL_FLOAT, 0, 0, BUFFER_OFFSET(0));
  glEnableVertexAttribArray(location);
}

//--------------------------------------------------------------------
void Cuboid::buildNormal(GLuint location) {
  mNormals.clear();
  mNormals.reserve(mVertices.size());
  for (int i = 0; i < 6; ++i) mNormals.push_back(glm::vec3(0, -1, 0));
  for (int i = 0; i < 6; ++i) mNormals.push_back(glm::vec3(0, 1, 0));
  for (int i = 0; i < 6; ++i) mNormals.push_back(glm::vec3(1, 0, 0));
  for (int i = 0; i < 6; ++i) mNormals.push_back(glm::vec3(-1, 0, 0));
  for (int i = 0; i < 6; ++i) mNormals.push_back(glm::vec3(0, 0, 1));
  for (int i = 0; i < 6; ++i) mNormals.push_back(glm::vec3(0, 0, -1));

  glGenBuffers(1, &mNormalBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, mNormalBuffer);
  glBufferData(GL_ARRAY_BUFFER, mNormals.size() * sizeof(glm::vec3),
    value_ptr(mNormals[0]), GL_STATIC_DRAW);

  glEnableVertexAttribArray(location);
  glVertexAttribPointer(location, 3, GL_FLOAT, 0, 0, BUFFER_OFFSET(0));
}

//--------------------------------------------------------------------
void Cuboid::buildTexcoord(GLuint location) {
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

  bindVertexArrayObject();

  glGenBuffers(1, &mTexcoordBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, mTexcoordBuffer);
  glBufferData(GL_ARRAY_BUFFER, mTexcoords.size() * sizeof(glm::vec2),
    value_ptr(mTexcoords[0]), GL_STATIC_DRAW);

  glEnableVertexAttribArray(location);
  glVertexAttribPointer(location, 2, GL_FLOAT, 0, 0, BUFFER_OFFSET(0));
}

//--------------------------------------------------------------------
void Cuboid::draw(GLuint primcount /* = 1*/) {
  bindVertexArrayObject();
  if (primcount == 1) {
    glDrawArrays(GL_TRIANGLES, 0, mVertices.size());
    //glDrawArrays(GL_TRIANGLES, 0, 12);
  } else {
    glDrawArraysInstanced(GL_TRIANGLES, 0, mVertices.size(), primcount);
  }
}
}
