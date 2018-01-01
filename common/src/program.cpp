#include "program.h"
#include "common.h"
#include <algorithm>
#include <functional>

namespace zxd {

//--------------------------------------------------------------------
GLint Program::getAttribLocation(const std::string& name) {
  return ::getAttribLocation(object, name);
}

//--------------------------------------------------------------------
void Program::setUniformLocation(GLint* location, const std::string& name) {
  ::setUniformLocation(location, object, name);
}

//--------------------------------------------------------------------
void Program::attachShaderFile(GLenum type, const std::string& file) {
  ::attachShaderFile(object, type, file);
}

//--------------------------------------------------------------------
void Program::attachShaderSource(GLenum type, const StringVector& source) {
  ::attachShaderSource(object, type, source);
}

//--------------------------------------------------------------------
void Program::attachShaderSourceAndFile(
  GLenum type, StringVector& source, const std::string& file) {
  ::attachShaderSourceAndFile(object, type, source, file);
}
}
