#ifndef DEBUGGER_H
#define DEBUGGER_H
#include "gl.h"
#include "glm.h"

#define BUFFER_OFFSET(bytes) ((GLubyte *)NULL + (bytes))

namespace debugger
{
  void draw_point(const glm::vec3& point, const glm::mat4& mvp, GLfloat size = 1,  const glm::vec4& color = glm::vec4(1));
  void draw_point(const zxd::vec3_vector& vertices, const glm::mat4& mvp, GLfloat size = 1, const glm::vec4& color = glm::vec4(1));
  void draw_line(const glm::vec3& p0, const glm::vec3& p1, const glm::mat4& mvp, GLfloat width = 1, const glm::vec4& color = glm::vec4(1));
  void draw_line(GLenum mode, const zxd::vec3_vector& vertices, const glm::mat4& mvp, GLfloat width = 1, const glm::vec4& color = glm::vec4(1));
}


#endif /* DEBUGGER_H */
