#ifndef COMMON_H
#define COMMON_H
#include "glad/glad.h"
#include <string>

#define BUFFER_OFFSET(bytes) ((GLubyte *)NULL + (bytes))

namespace zxd {

void setUniformLocation(GLint *loc, GLint program, const std::string &name);

std::string readFile(const std::string &filepath);

void matrixAttribPointer(
  GLint index, GLuint divisor = 1, GLboolean normalize = GL_FALSE);
}

#endif /* COMMON_H */
