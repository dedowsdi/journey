#ifndef GL4_GLC_SHADER_H
#define GL4_GLC_SHADER_H

#include <gl.h>
#include <list>
#include <string>
#include <memory>
#include <map>

namespace zxd
{

class shader_source
{
public:

  shader_source& add_file(const std::string& filename, bool append = true);

  shader_source& add_string(const std::string& filename, bool append = true);

  shader_source& replace_place_holder(
    const std::map<std::string, std::string>& replace_map);

  std::string get_final_source() const;

private:
  std::list<std::string> _sources;
};

class shader
{
public:

  shader(GLenum type);
  ~shader();

  // no copy, no move
  shader(const shader&) = delete;
  shader& operator=(const shader&) = delete;

  shader& source(const std::string& s);

  bool compile();

  GLuint get_object() const { return _object; }

  GLenum get_type() const;

  GLint get_iv(GLenum pname) const;

  std::string get_info_log() const;

  std::string get_source() const;

private:

  GLuint _object;
};

}

#endif /* GL4_GLC_SHADER_H */
