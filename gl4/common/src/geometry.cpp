#include "geometry.h"
#include "common.h"

namespace zxd {

//--------------------------------------------------------------------
void Geometry::bind(
  GLint vertex, GLint normal /* = -1*/, GLint texcoord /* = -1*/) {
  bindVertexArrayObject();

  if (vertex != -1) {
    glGenBuffers(1, &mVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(glm::vec3),
      value_ptr(mVertices[0]), GL_STATIC_DRAW);

    glVertexAttribPointer(vertex, 3, GL_FLOAT, 0, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(vertex);
  }

  if (normal != -1) {
    glGenBuffers(1, &mNormalBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mNormalBuffer);
    glBufferData(GL_ARRAY_BUFFER, mNormals.size() * sizeof(glm::vec3),
      value_ptr(mNormals[0]), GL_STATIC_DRAW);

    glVertexAttribPointer(normal, 3, GL_FLOAT, 0, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(normal);
  }

  if (texcoord != -1) {
    glGenBuffers(1, &mTexcoordBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mTexcoordBuffer);
    glBufferData(GL_ARRAY_BUFFER, mTexcoords.size() * sizeof(glm::vec2),
      value_ptr(mTexcoords[0]), GL_STATIC_DRAW);

    glVertexAttribPointer(texcoord, 2, GL_FLOAT, 0, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(texcoord);
  }
}

//--------------------------------------------------------------------
void Geometry::buildMesh(
  GLboolean normal /* = 1*/, GLboolean texcoord /* = 1*/) {
  buildVertex();
  if (normal) {
    buildNormal();
  }
  if (texcoord) {
    buildTexcoord();
  }
}

//--------------------------------------------------------------------
void Geometry::bindVertexArrayObject() {
  if (mVao == -1) {
    glGenVertexArrays(1, &mVao);
  }
  glBindVertexArray(mVao);
}
}
