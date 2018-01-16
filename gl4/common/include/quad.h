#ifndef QUAD_H
#define QUAD_H
#include "program.h"
#include "glm.h"
#include <array>
#include "common.h"

namespace zxd {

struct QuadProgram : public zxd::Program {
  GLint loc_quadMap;
  GLint attrib_vertex;
  GLint attrib_texcoord;

  QuadProgram() {}

  void attachShaders() {
    attachShaderFile(GL_VERTEX_SHADER, "data/shader/quad.vs.glsl");
    attachShaderFile(GL_FRAGMENT_SHADER, "data/shader/quad.fs.glsl");
  }

  void updateUniforms(GLuint texIndex = 0) {
    glUniform1i(loc_quadMap, texIndex);
  }

  virtual void bindUniformLocations() {
    setUniformLocation(&loc_quadMap, "quadMap");
  }

  virtual void bindAttribLocations() {
    attrib_vertex = getAttribLocation("vertex");
    attrib_texcoord = getAttribLocation("texcoord");
  }
};

void drawQuad(GLuint texture);

// triangle strip quad
struct Quad {
  GLuint vao;
  // clang-format off
  std::array<glm::vec2, 4> vertices{
    glm::vec2(-1.0f, 1.0f),
    glm::vec2(-1.0f, -1.0f),
    glm::vec2(1.0f, 1.0f),
    glm::vec2(1.0f, -1.0f) 
  };
  std::array<glm::vec2, 4> texcoords{
    glm::vec2{0.0f, 1.0f},
    glm::vec2{0.0f, 0.0f}, 
    glm::vec2{1.0f, 1.0f}, 
    glm::vec2{1.0f, 0.0f}
  };
  std::array<glm::vec3, 4> normals{
    glm::vec3{0.0f, 0.0f, 1.0f}, 
    glm::vec3{0.0f, 0.0f, 1.0f}, 
    glm::vec3{0.0f, 0.0f, 1.0f},
    glm::vec3{0.0f, 0.0f, 1.0f}
  };
  std::array<glm::vec3, 4> tangents{
    glm::vec3{1.0f, 0.0f, 0.0f }, 
    glm::vec3{1.0f, 0.0f, 0.0f }, 
    glm::vec3{1.0f, 0.0f, 0.0f },
    glm::vec3{1.0f, 0.0f, 0.0f }
  };
  // clang-format on

  void draw() {
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  }

  void setupVertexAttrib(GLint attribVertex, GLint attribTexcoord = -1,
    GLint attribNormal = -1, GLint attribTangent = -1) {
    if (!glIsVertexArray(vao)) 
      glGenVertexArrays(1, &vao);

    glBindVertexArray(vao);
    if (attribVertex != -1) zxd::setupVertexAttrib(attribVertex, vertices);
    if (attribTexcoord != -1) zxd::setupVertexAttrib(attribTexcoord, texcoords);
    if (attribNormal != -1) zxd::setupVertexAttrib(attribNormal, normals);
    if (attribTangent != -1) zxd::setupVertexAttrib(attribTangent, tangents);
  }
};
}

#endif /* QUAD_H */
