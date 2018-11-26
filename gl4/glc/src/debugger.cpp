#include "debugger.h"

#include "program.h"

namespace debugger
{
//--------------------------------------------------------------------
void debugger_drawable::init_vao(GLuint components)
{
  if(vao != -1)
    return;

  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);

  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, buffer_size, 0, GL_DYNAMIC_DRAW);

  glVertexAttribPointer(0, components, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
  glEnableVertexAttribArray(0);
}

//--------------------------------------------------------------------
void debugger_drawable::update_buffer(GLuint size, const GLvoid* data)
{
  assert(vbo != -1);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  if(buffer_size < size)
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW);
  else
    glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
  buffer_size = size;
}

//--------------------------------------------------------------------
void debugger_drawable::bind_vao()
{
  assert(vao != -1);
  glBindVertexArray(vao);
}

}
