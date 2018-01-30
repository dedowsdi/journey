#include "quad.h"
#include "common.h"

namespace zxd {

//--------------------------------------------------------------------
void draw_quad(GLuint texture) {
  static GLuint vao;
  static GLuint vbo;
  static quad_program quad_program;
  // clang-format off
  // {vertex.x, vertex.y, texcoord.x, texcoord.y}
  static GLfloat vertices[6][4] = {
    {-1, -1, 0, 0},
    {1,  -1, 1, 0},
    {1,  1, 1, 1},

    {-1, -1, 0, 0},
    {1,  1, 1, 1},
    {-1, 1, 0, 1},
  };
  // clang-format on

  if (quad_program.object == -1) {
    quad_program.init();

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(
      GL_ARRAY_BUFFER, sizeof(vertices), vertices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(quad_program.al_vertex, 2, GL_FLOAT, GL_FALSE,
      4 * sizeof(GLfloat), BUFFER_OFFSET(0));
    glEnableVertexAttribArray(quad_program.al_vertex);

    glVertexAttribPointer(quad_program.al_texcoord, 2, GL_FLOAT, GL_FALSE,
      4 * sizeof(GLfloat), BUFFER_OFFSET(2 * sizeof(GLfloat)));
    glEnableVertexAttribArray(quad_program.al_texcoord);
  }

  glBindVertexArray(vao);
  glUseProgram(quad_program);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture);

  quad_program.update_uniforms(0);

  glDrawArrays(GL_TRIANGLES, 0, 6);
}
}
