#include "program.h"

#include <iostream>
#include <algorithm>
#include <functional>
#include <sstream>
#include <memory>

#include "shader.h"
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
  if(_load_callback)
    _load_callback();
  bind_attrib_locations(); // bind must be called before link, different from get
  link();
  bind_uniform_locations();
}

//--------------------------------------------------------------------
void program::reload()
{
  std::cout << "reloading program " << get_print_name() << std::endl;
  clear();
  init();
}


//--------------------------------------------------------------------
void program::bind_attrib_location(GLuint index, const std::string& name)
{
  glBindAttribLocation(_object, index, name.c_str());
}

//--------------------------------------------------------------------
void program::link()
{
  assert(get_iv(GL_ATTACHED_SHADERS) == _shaders.size());
  glLinkProgram(_object);

  if (!get_iv(GL_LINK_STATUS))
  {
    std::cerr << get_info_log() << std::endl;
  }

  std::cout << get_print_name() <<  " link ";
  for (const auto& item : _shaders)
  {
    std::cout << gl_shader_type_to_string(item.first) << " ";
  }
  std::cout << std::endl;
}

//--------------------------------------------------------------------
void program::use()
{
  if(!is_inited())
    init();
  glUseProgram(_object);
}

//--------------------------------------------------------------------
GLint program::get_attrib_location(const std::string& name)
{
  GLint location = glGetAttribLocation(_object, name.c_str());
  if (location == -1)
  {
    std::cout << get_print_name() << " failed to get attribute location : \""
              << name << "\"" << std::endl;
  }
  return location;
}

//--------------------------------------------------------------------
GLint program::get_uniform_location(const std::string& name)
{
  auto location = glGetUniformLocation(_object, name.c_str());
  if (location == -1)
  {
    std::cout << _object << " : " << _name
              << " failed to get uniform location : \"" << name << "\""
              << std::endl;
  }
  return location;
}

//--------------------------------------------------------------------
void program::attach(GLenum type, const std::string& file,
  const std::map<std::string, std::string>& replace_map)
{
  string_vector sv;
  attach(type, sv, file, replace_map);
}

//--------------------------------------------------------------------
void program::attach(GLenum type, const string_vector& source)
{
  auto sh = std::make_shared<shader>(type);
  std::stringstream ss;
  for (const auto& s : source)
  {
    ss << s;
  }
  sh->source(ss.str());
  sh->compile();
  attach(sh);
}

//--------------------------------------------------------------------
void program::attach(GLenum type, const string_vector& source,
  const std::string& file,
  const std::map<std::string, std::string>& replace_map)
{

  shader_source src;
  for (const auto& s : source)
  {
    src.add_string(s);
  }
  src.add_file(file);
  src.replace_place_holder(replace_map);

  string_vector sv{src.get_final_source()};

  attach(type, sv);
}

//--------------------------------------------------------------------
void program::attach(const std::shared_ptr<shader>& s)
{
  auto type = s->get_type();
  auto iter = _shaders.find(type);
  if (iter != _shaders.end())
  {
    std::cerr << get_print_name() << " : replace "
              << gl_shader_type_to_string(type) << std::endl;
  }

  _shaders.insert(std::make_pair(type, s));
  glAttachShader(_object, s->get_object());
}


//--------------------------------------------------------------------
GLint program::get_iv(GLenum pname) const
{
  GLint v;
  glGetProgramiv(_object, pname, &v);
  return v;
}

//--------------------------------------------------------------------
std::string program::get_info_log() const
{
  auto len = get_iv(GL_INFO_LOG_LENGTH);
  if (len == 0)
  {
    std::stringstream ss;
    ss << " link failed, and has no log, good luck!" << std::endl;
    return ss.str();
  }
  else
  {
    auto log = std::make_unique<char[]>(len + 1);
    glGetProgramInfoLog(_object, len, 0, log.get());
    return std::string(log.get());
  }
}

//--------------------------------------------------------------------
void program::print_shader_sources()
{
  constexpr GLsizei max_shaders = 16;
  std::array<GLuint, max_shaders> shaders;
  GLsizei num_shaders;
  glGetAttachedShaders(_object, max_shaders, &num_shaders, &shaders.front());

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
  if (glIsProgram(_object))
  {
    glDeleteProgram(_object);
  }
}

//--------------------------------------------------------------------
void program::uniform1f(const std::string &name, GLfloat v0)
{
  glUniform1f(get_uniform_location(name), v0);
}

//--------------------------------------------------------------------
void program::uniform2f(const std::string& name, GLfloat v0, GLfloat v1)
{
  glUniform2f(get_uniform_location(name), v0, v1);
}

//--------------------------------------------------------------------
void program::uniform3f(
  const std::string& name, GLfloat v0, GLfloat v1, GLfloat v2)
{
  glUniform3f(get_uniform_location(name), v0, v1, v2);
}

//--------------------------------------------------------------------
void program::uniform4f(
  const std::string& name, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3)
{
  glUniform4f(get_uniform_location(name), v0, v1, v2, v3);
}

//--------------------------------------------------------------------
void program::uniform1i(const std::string& name, GLint v0)
{
  glUniform1i(get_uniform_location(name), v0);
}

//--------------------------------------------------------------------
void program::uniform2i(const std::string& name, GLint v0, GLint v1)
{
  glUniform2i(get_uniform_location(name), v0, v1);
}

//--------------------------------------------------------------------
void program::uniform3i(const std::string& name, GLint v0, GLint v1, GLint v2)
{
  glUniform3i(get_uniform_location(name), v0, v1, v2);
}

//--------------------------------------------------------------------
void program::uniform1fv(
  const std::string& name, GLsizei count, const GLfloat* value)
{
  glUniform1fv(get_uniform_location(name), count, value);
}

//--------------------------------------------------------------------
void program::uniform2fv(
  const std::string& name, GLsizei count, const GLfloat* value)
{
  glUniform2fv(get_uniform_location(name), count, value);
}

//--------------------------------------------------------------------
void program::uniform3fv(
  const std::string& name, GLsizei count, const GLfloat* value)
{
  glUniform3fv(get_uniform_location(name), count, value);
}

//--------------------------------------------------------------------
void program::uniform4fv(
  const std::string& name, GLsizei count, const GLfloat* value)
{
  glUniform4fv(get_uniform_location(name), count, value);
}

//--------------------------------------------------------------------
void program::uniform1iv(
  const std::string& name, GLsizei count, const GLint* value)
{
  glUniform1iv(get_uniform_location(name), count, value);
}

//--------------------------------------------------------------------
void program::uniform2iv(
  const std::string& name, GLsizei count, const GLint* value)
{
  glUniform2iv(get_uniform_location(name), count, value);
}

//--------------------------------------------------------------------
void program::uniform3iv(
  const std::string& name, GLsizei count, const GLint* value)
{
  glUniform3iv(get_uniform_location(name), count, value);
}

//--------------------------------------------------------------------
void program::uniform4iv(
  const std::string& name, GLsizei count, const GLint* value)
{
  glUniform4iv(get_uniform_location(name), count, value);
}

//--------------------------------------------------------------------
void program::uniform4i(
  const std::string& name, GLint v0, GLint v1, GLint v2, GLint v3)
{
  glUniform4i(get_uniform_location(name), v0, v1, v2, v3);
}

void program::create_program() { _object = glCreateProgram(); }

//--------------------------------------------------------------------
std::string program::get_print_name()
{
  std::stringstream ss;
  ss << _object << "(" << _name << ")";
  return ss.str();
}

}
