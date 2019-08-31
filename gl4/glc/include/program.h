#ifndef GL_GLC_PROGRAM_H
#define GL_GLC_PROGRAM_H

#include "gl.h"
#include "glm.h"
#include <string>
#include <vector>
#include <map>

typedef std::vector<std::string> string_vector;
typedef std::vector<const char*> cstring_vector;

namespace zxd
{

void add_shader_content(string_vector& v, const std::string& content);

class program
{
public:
  GLuint object;
  // some popular matrix

protected:
  std::string m_name;

public:
  program() : object(-1) {}
  virtual ~program() {}

  // to use this directly in gl procedure
  operator GLuint() { return object; }

  virtual void init();

  virtual void reload();

  bool is_inited() { return object != -1; }

  void link();

  void use();

  void bind_attrib_location(GLuint index, const std::string& name);

  GLint get_attrib_location(const std::string& name);

  GLint get_uniform_location(const std::string& name);

  void attach(GLenum type, const std::string& file,
    const std::map<std::string, std::string>& replace_map = {});
  bool attach(GLenum type, const string_vector& source);
  void attach(GLenum type, const string_vector& source, const std::string& file,
    const std::map<std::string, std::string>& replace_map = {});
  void print_shader_sources();

  void clear();

  void uniform1f(const std::string& name, GLfloat v0);
  void uniform2f(const std::string& name, GLfloat v0, GLfloat v1);
  void uniform3f(const std::string& name, GLfloat v0, GLfloat v1, GLfloat v2);
  void uniform4f(
    const std::string& name, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
  void uniform1i(const std::string& name, GLint v0);
  void uniform2i(const std::string& name, GLint v0, GLint v1);
  void uniform3i(const std::string& name, GLint v0, GLint v1, GLint v2);
  void uniform1fv(const std::string& name, GLsizei count, const GLfloat* value);
  void uniform2fv(const std::string& name, GLsizei count, const GLfloat* value);
  void uniform3fv(const std::string& name, GLsizei count, const GLfloat* value);
  void uniform4fv(const std::string& name, GLsizei count, const GLfloat* value);
  void uniform1iv(const std::string& name, GLsizei count, const GLint* value);
  void uniform2iv(const std::string& name, GLsizei count, const GLint* value);
  void uniform3iv(const std::string& name, GLsizei count, const GLint* value);
  void uniform4iv(const std::string& name, GLsizei count, const GLint* value);
  void uniform4i(
    const std::string& name, GLint v0, GLint v1, GLint v2, GLint v3);

  std::string name() const { return m_name; }
  void name(std::string v) { m_name = v; }

private:

  virtual void attach_shaders(){};
  virtual void bind_uniform_locations(){};
  virtual void bind_attrib_locations(){}

  void create_program();

};
}

#endif /* GL_GLC_PROGRAM_H */
