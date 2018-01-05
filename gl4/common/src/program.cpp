#include "program.h"
#include <algorithm>
#include <functional>
#include "glEnumString.h"
#include "common.h"
#include <sstream>

namespace zxd {

//--------------------------------------------------------------------
void Program::link() {
  glLinkProgram(object);
  GLint status;
  glGetProgramiv(object, GL_LINK_STATUS, &status);
  if (!status) {
    GLint len;
    glGetProgramiv(object, GL_INFO_LOG_LENGTH, &len);
    if (len == 0) {
      std::stringstream ss;
      ss << "program " << object << " link failed, and has no log, good luck!"
         << std::endl;
      std::cout << ss.str() << std::endl;
    } else {
      char* log = static_cast<char*>(malloc(len + 1));
      glGetProgramInfoLog(object, len, 0, log);
      std::cout << log << std::endl;
    }
  }
}

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
  if (!attachShaderSource(type, sv))
    std::cout << "faild to compile " << file << std::endl;
}

//--------------------------------------------------------------------
bool Program::attachShaderSource(GLenum type, const StringVector& source) {
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
    return false;
  }
  glAttachShader(object, sh);
  glDeleteShader(sh);
  return true;
}

//--------------------------------------------------------------------
void Program::attachShaderSourceAndFile(
  GLenum type, StringVector& source, const std::string& file) {
  source.push_back(readFile(file));
  if (!attachShaderSource(type, source))
    std::cout << "failed to compile " << file << std::endl;
}
}
