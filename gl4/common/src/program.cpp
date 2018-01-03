#include "program.h"
#include <algorithm>
#include <functional>
#include "glEnumString.h"
#include "common.h"

namespace zxd {

//--------------------------------------------------------------------
GLint Program::getAttribLocation(const std::string& name) {
  GLint location = glGetAttribLocation(object, name.c_str());
  if (location == -1) {
    std::cout << "failed to get attribute location : " << name << std::endl;
  }
  return location;
}

//--------------------------------------------------------------------
void Program::setUniformLocation(GLint* location, const std::string& name) {
  *location = glGetUniformLocation(object, name.c_str());
  if (*location == -1) {
    fprintf(stderr, "failed to get uniform location : %s\n", name.c_str());
  }
}

//--------------------------------------------------------------------
void Program::attachShaderFile(GLenum type, const std::string& file) {
  StringVector sv;
  sv.push_back(readFile(file));
  attachShaderSource(type, sv);
}

//--------------------------------------------------------------------
void Program::attachShaderSource(GLenum type, const StringVector& source) {
  GLuint sh;

  sh = glCreateShader(type);

  CStringVector csv;
  std::transform(source.begin(), source.end(), std::back_inserter(csv),
    std::mem_fn(&std::string::c_str));

  glShaderSource(sh, csv.size(), &csv[0], NULL);

  glCompileShader(sh);

  GLint compiled;
  // check if compile failed
  glGetShaderiv(sh, GL_COMPILE_STATUS, &compiled);
  if (!compiled) {
    GLsizei len;
    glGetShaderiv(sh, GL_INFO_LOG_LENGTH, &len);

    GLchar* log = (GLchar*)malloc(len + 1);
    glGetShaderInfoLog(sh, len, &len, log);
    printf("%s  compilation failed: %s", glShaderTypeToString(type), log);
    free(log);
    return;
  }
  glAttachShader(object, sh);
  glDeleteShader(sh);
}

//--------------------------------------------------------------------
void Program::attachShaderSourceAndFile(
  GLenum type, StringVector& source, const std::string& file) {
  source.push_back(readFile(file));
  attachShaderSource(type, source);
}
}
