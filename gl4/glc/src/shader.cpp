#include <shader.h>
#include <exception.h>
#include <glenumstring.h>
#include <stream_util.h>
#include <string_util.h>
#include <exception.h>

namespace zxd
{

//--------------------------------------------------------------------
shader_source& shader_source::add_file(const std::string& filename, bool append)
{
  return add_string(stream_util::read_resource(filename), append);
}

//--------------------------------------------------------------------
shader_source& shader_source::add_string(const std::string& s, bool append)
{
  if (append)
    _sources.push_back(s);
  else
    _sources.push_front(s);
  return *this;
}

//--------------------------------------------------------------------
shader_source& shader_source::replace_place_holder(
  const std::map<std::string, std::string>& replace_map)
{
  for (auto& s : _sources)
  {
    for (const auto& p : replace_map)
      s = string_util::replace_string(s, p.first, p.second);
  }
  return *this;
}

//--------------------------------------------------------------------
std::string shader_source::get_final_source() const
{
  if (_sources.empty())
  {
    throw gl_empty_source{};
  }

  std::stringstream ss;
  ss << _sources.front();

  static const std::string version = "#version";

  // comment // for non leading source
  for (auto it = ++_sources.begin(); it != _sources.end(); ++it)
  {
    if (it->size() > version.size() && it->substr(0, version.size()) == version)
    {
      ss << "// ";
    }
    ss << *it;
  }

  return ss.str();
}

//--------------------------------------------------------------------
shader::shader(GLenum type)
{
  _object = glCreateShader(type);
}

//--------------------------------------------------------------------
shader::~shader()
{
  glDeleteShader(_object);
}

//--------------------------------------------------------------------
bool shader::compile()
{
  glCompileShader(_object);

  if (!get_iv(GL_COMPILE_STATUS))
  {
    std::cout << gl_shader_type_to_string(get_type())
              << " compilation failed : " << get_info_log() << std::endl;
    std::cout << "------------------------------" << std::endl;
    std::cout << get_source() << std::endl;
    return false;
  }

  return true;
}

//--------------------------------------------------------------------
shader& shader::source(const std::string& s)
{
  auto cs = s.c_str();
  auto size = static_cast<GLint>(s.size());
  glShaderSource(_object, 1, &cs, &size);
  return *this;
}

//--------------------------------------------------------------------
GLenum shader::get_type() const
{
  return get_iv(GL_SHADER_TYPE);
}

//--------------------------------------------------------------------
GLint shader::get_iv(GLenum pname) const
{
  GLint v;
  glGetShaderiv(_object, pname, &v);
  return v;
}

//--------------------------------------------------------------------
std::string shader::get_info_log() const
{
  auto len = get_iv(GL_INFO_LOG_LENGTH);
  auto log = std::make_unique<char[]>(len + 1);
  glGetShaderInfoLog(_object, len, 0, log.get());
  return std::string(log.get());
}

//--------------------------------------------------------------------
std::string shader::get_source() const
{
  auto len = get_iv(GL_SHADER_SOURCE_LENGTH);
  auto source = std::make_unique<char[]>(len + 1);
  glGetShaderSource(_object, len, 0, source.get());
  return std::string(source.get());
}

}
