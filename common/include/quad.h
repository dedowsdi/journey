#ifndef QUAD_H
#define QUAD_H
#include "program.h"

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
}

#endif /* QUAD_H */
