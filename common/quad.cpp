#include "quad.h"
#include "common.h"

namespace zxd {

//--------------------------------------------------------------------
void drawQuad(GLuint texture) {
  static GLuint vao;
  static GLuint vbo;
  static QuadProgram quadProgram;
  // clang-format off
  // {vertex.x, vertex.y, texcoord.x, texcoord.y}
  static GLfloat vertices[4][4] = {
    {-1, -1, 0, 0},
    {1,  -1, 1, 0},
    {1,  1, 1, 1},
    {-1, 1, 0, 1},
  };
  // clang-format on

  if (quadProgram.object == -1) {
    quadProgram.init();

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(
      GL_ARRAY_BUFFER, sizeof(vertices), vertices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(quadProgram.attrib_vertex, 2, GL_FLOAT, GL_FALSE,
      4 * sizeof(GLfloat), BUFFER_OFFSET(0));
    glEnableVertexAttribArray(quadProgram.attrib_vertex);

    glVertexAttribPointer(quadProgram.attrib_texcoord, 2, GL_FLOAT, GL_FALSE,
      4 * sizeof(GLfloat), BUFFER_OFFSET(2 * sizeof(GLfloat)));
    glEnableVertexAttribArray(quadProgram.attrib_texcoord);
  }

  glBindVertexArray(vao);
  glUseProgram(quadProgram);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture);

  quadProgram.updateUniforms(0);

  glDrawArrays(GL_QUADS, 0, 4);

}
}
