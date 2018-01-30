#include "program.h"
#include <algorithm>
#include <functional>
#include "glEnumString.h"
#include "common.h"
#include <sstream>

namespace zxd {

//--------------------------------------------------------------------
void program::link() {
  glLinkProgram(object);
  GLint status;
  glGetProgramiv(object, GL_LINK_STATUS, &status);
  if (!status) {
    GLint len;
    glGetProgramiv(object, GL_INFO_LOG_LENGTH, &len);
    if (len == 0) {
      std::stringstream ss;
      ss << "program " << object << " : " << m_name
         << " link failed, and has no log, good luck!" << std::endl;
      std::cout << ss.str() << std::endl;
    } else {
      char* log = static_cast<char*>(malloc(len + 1));
      glGetProgramInfoLog(object, len, 0, log);
      std::cout << log << std::endl;
    }
  }
}

//--------------------------------------------------------------------
GLint program::attrib_location(const std::string& name) {
  GLint location = glGetAttribLocation(object, name.c_str());
  if (location == -1) {
    std::cout << object << " : " << m_name
              << " failed to get attribute location : " << name << std::endl;
  }
  return location;
}

//--------------------------------------------------------------------
void program::uniform_location(GLint* location, const std::string& name) {
  *location = glGetUniformLocation(object, name.c_str());
  if (*location == -1) {
    std::cout << object << " : " << m_name
              << " failed to get uniform location : " << name << std::endl;
  }
}

//--------------------------------------------------------------------
void program::attach_shader_file(GLenum type, const std::string& file) {
  string_vector sv;
  sv.push_back(read_file(file));
  if (!attach_shader_source(type, sv))
    std::cout << "faild to compile " << file << std::endl;
}

//--------------------------------------------------------------------
bool program::attach_shader_source(GLenum type, const string_vector& source) {
  GLuint sh;

  sh = glCreateShader(type);

  cstring_vector csv;
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
    printf("%s  compilation failed: %s", gl_shader_type_to_string(type), log);
    free(log);
    return false;
  }
  glAttachShader(object, sh);
  glDeleteShader(sh);
  return true;
}

//--------------------------------------------------------------------
void program::attach_shader_source_and_file(
  GLenum type, const string_vector& source, const std::string& file) {
  string_vector combinedSource(source);
  combinedSource.push_back(read_file(file));
  if (!attach_shader_source(type, combinedSource))
    std::cout << "failed to compile " << file << std::endl;
}

//--------------------------------------------------------------------
void program::clear() {
  if (glIsProgram(object)) {
    glDeleteProgram(object);
  }
}
}
