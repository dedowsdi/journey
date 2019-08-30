#include "program.h"

#include <iostream>
#include <algorithm>
#include <functional>
#include <sstream>
#include <memory>

#include "glenumstring.h"
#include "common.h"
#include "stream_util.h"
#include "string_util.h"

namespace zxd
{

//--------------------------------------------------------------------
void add_shader_content(string_vector& v, const std::string& content)
{
  // commment leading version if current strings is not empty
  if (!v.empty() && v.front().size() > 0)
  {
    std::string version = "#version";
    if (content.size() > version.size() &&
        content.substr(0, version.size()) == version)
    {
      v.push_back("//");
    }
  }

  v.push_back(content);
}

//--------------------------------------------------------------------
void program::init()
{
  create_program();
  attach_shaders();
  bind_attrib_locations(); // bind must be called before link, different from get
  link();
  bind_uniform_locations();
}

//--------------------------------------------------------------------
void program::reload()
{
  std::cout << "reloading shader " << m_name << std::endl;
  clear();
  init();
}


//--------------------------------------------------------------------
void program::bind_attrib_location(GLuint index, const std::string& name)
{
  glBindAttribLocation(object, index, name.c_str());
}

//--------------------------------------------------------------------
void program::link()
{
  glLinkProgram(object);
  GLint status;
  glGetProgramiv(object, GL_LINK_STATUS, &status);
  if (!status)
  {
    GLint len;
    glGetProgramiv(object, GL_INFO_LOG_LENGTH, &len);
    if (len == 0)
    {
      std::stringstream ss;
      ss << "program " << object << " : " << m_name
         << " link failed, and has no log, good luck!" << std::endl;
      std::cout << ss.str() << std::endl;
    } else
    {
      char* log = static_cast<char*>(malloc(len + 1));
      glGetProgramInfoLog(object, len, 0, log);
      std::cout << log << std::endl;
      print_shader_sources();
    }
  }

  GLint num_shaders;
  glGetProgramiv(object, GL_ATTACHED_SHADERS, &num_shaders);
  constexpr GLsizei max_shaders = 64;
  std::array<GLuint, max_shaders> shaders;
  glGetAttachedShaders(object, max_shaders, &num_shaders, &shaders.front());

  std::cout << "link ";
  for (int i = 0; i < num_shaders; ++i)
  {
    GLint shader_type;
    glGetShaderiv(shaders[i], GL_SHADER_TYPE, &shader_type);
    std::cout << gl_shader_type_to_string(shader_type) << " ";
  }
  std::cout << std::endl;
}

//--------------------------------------------------------------------
void program::use()
{
  if(!is_inited())
    init();
  glUseProgram(object);
}

//--------------------------------------------------------------------
GLint program::attrib_location(const std::string& name)
{
  GLint location = glGetAttribLocation(object, name.c_str());
  if (location == -1)
  {
    std::cout << object << " : " << m_name
              << " failed to get attribute location : \"" << name << "\""
              << std::endl;
  }
  return location;
}

//--------------------------------------------------------------------
void program::uniform_location(GLint* location, const std::string& name)
{
  *location = glGetUniformLocation(object, name.c_str());
  if (*location == -1)
  {
    std::cout << object << " : " << m_name
              << " failed to get uniform location : \"" << name << "\""
              << std::endl;
  }
}

//--------------------------------------------------------------------
void program::attach(GLenum type, const std::string& file,
  const std::map<std::string, std::string>& replace_map)
{
  string_vector sv;
  attach(type, sv, file, replace_map);
}

//--------------------------------------------------------------------
bool program::attach(GLenum type, const string_vector& source)
{
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
  if (!compiled)
  {
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
void program::attach(GLenum type, const string_vector& source,
  const std::string& file,
  const std::map<std::string, std::string>& replace_map)
{
  string_vector combined_source(source);
  add_shader_content(combined_source, stream_util::read_resource(file));

  for (auto& str : combined_source)
  {
    for (const auto& p : replace_map)
      str = string_util::replace_string(str, p.first, p.second);
  }

  if (!attach(type, combined_source))
  {
    std::cout << "failed to compile " << file << " : " <<  std::endl;
    std::cout << "******************************" << std::endl;
    std::copy(combined_source.begin(), combined_source.end(),
      std::ostream_iterator<std::string>(std::cout, ""));
    std::cout << stream_util::read_resource(file) << std::endl;
    std::cout << "******************************" << std::endl;
  }
}

//--------------------------------------------------------------------
void program::print_shader_sources()
{
  constexpr GLsizei max_shaders = 64;
  std::array<GLuint, max_shaders> shaders;
  GLsizei num_shaders;
  glGetAttachedShaders(object, max_shaders, &num_shaders, &shaders.front());

  std::cout << "============================================================"
            << std::endl;
  std::cout << "program shaders : " << std::endl;
  for (int i = 0; i < num_shaders; ++i)
  {
    GLint shader_type;
    glGetShaderiv(shaders[i], GL_SHADER_TYPE, &shader_type);
    std::cout << gl_shader_type_to_string(shader_type) << "\n"
              << "******************************\n" ;
    GLint shader_length;
    glGetShaderiv(shaders[i], GL_SHADER_SOURCE_LENGTH, &shader_length);
    auto source = std::make_unique<GLchar>(shader_length+1);
    glGetShaderSource(shaders[i], shader_length+1, 0, source.get());
    std::cout << source.get() << std::endl;
  }
  std::cout << "============================================================"
            << std::endl;
}

//--------------------------------------------------------------------
void program::clear()
{
  if (glIsProgram(object))
  {
    glDeleteProgram(object);
  }
}

//--------------------------------------------------------------------
void program::uniform1f(const std::string &name, GLfloat v0)
{
  GLint location;
  uniform_location(&location, name);
  if (location != -1)
  {
    glUniform1f(location, v0);
  }
}

//--------------------------------------------------------------------
void program::uniform2f(const std::string& name, GLfloat v0, GLfloat v1)
{
  GLint location;
  uniform_location(&location, name);
  if (location != -1)
  {
    glUniform2f(location, v0, v1);
  }
}

//--------------------------------------------------------------------
void program::uniform3f(
  const std::string& name, GLfloat v0, GLfloat v1, GLfloat v2)
{
  GLint location;
  uniform_location(&location, name);
  if (location != -1)
  {
    glUniform3f(location, v0, v1, v2);
  }
}

//--------------------------------------------------------------------
void program::uniform4f(
  const std::string& name, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3)
{
  GLint location;
  uniform_location(&location, name);
  if (location != -1)
  {
    glUniform4f(location, v0, v1, v2, v3);
  }
}

//--------------------------------------------------------------------
void program::uniform1i(const std::string& name, GLint v0)
{
  GLint location;
  uniform_location(&location, name);
  if (location != -1)
  {
    glUniform1i(location, v0);
  }
}

//--------------------------------------------------------------------
void program::uniform2i(const std::string& name, GLint v0, GLint v1)
{
  GLint location;
  uniform_location(&location, name);
  if (location != -1)
  {
    glUniform2i(location, v0, v1);
  }
}

//--------------------------------------------------------------------
void program::uniform3i(const std::string& name, GLint v0, GLint v1, GLint v2)
{
  GLint location;
  uniform_location(&location, name);
  if (location != -1)
  {
    glUniform3i(location, v0, v1, v2);
  }
}

//--------------------------------------------------------------------
void program::uniform1fv(
  const std::string& name, GLsizei count, const GLfloat* value)
{
  GLint location;
  uniform_location(&location, name);
  if (location != -1)
  {
    glUniform1fv(location, count, value);
  }
}

//--------------------------------------------------------------------
void program::uniform2fv(
  const std::string& name, GLsizei count, const GLfloat* value)
{
  GLint location;
  uniform_location(&location, name);
  if (location != -1)
  {
    glUniform2fv(location, count, value);
  }
}

//--------------------------------------------------------------------
void program::uniform3fv(
  const std::string& name, GLsizei count, const GLfloat* value)
{
  GLint location;
  uniform_location(&location, name);
  if (location != -1)
  {
    glUniform3fv(location, count, value);
  }
}

//--------------------------------------------------------------------
void program::uniform4fv(
  const std::string& name, GLsizei count, const GLfloat* value)
{
  GLint location;
  uniform_location(&location, name);
  if (location != -1)
  {
    glUniform4fv(location, count, value);
  }
}

//--------------------------------------------------------------------
void program::uniform1iv(
  const std::string& name, GLsizei count, const GLint* value)
{
  GLint location;
  uniform_location(&location, name);
  if (location != -1)
  {
    glUniform1iv(location, count, value);
  }
}

//--------------------------------------------------------------------
void program::uniform2iv(
  const std::string& name, GLsizei count, const GLint* value)
{
  GLint location;
  uniform_location(&location, name);
  if (location != -1)
  {
    glUniform2iv(location, count, value);
  }
}

//--------------------------------------------------------------------
void program::uniform3iv(
  const std::string& name, GLsizei count, const GLint* value)
{
  GLint location;
  uniform_location(&location, name);
  if (location != -1)
  {
    glUniform3iv(location, count, value);
  }
}

//--------------------------------------------------------------------
void program::uniform4iv(
  const std::string& name, GLsizei count, const GLint* value)
{
  GLint location;
  uniform_location(&location, name);
  if (location != -1)
  {
    glUniform4iv(location, count, value);
  }
}

//--------------------------------------------------------------------
void program::uniform4i(
  const std::string& name, GLint v0, GLint v1, GLint v2, GLint v3)
{
  GLint location;
  uniform_location(&location, name);
  if (location != -1)
  {
    glUniform4i(location, v0, v1, v2, v3);
  }
}

}
