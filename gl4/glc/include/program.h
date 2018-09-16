#ifndef PROGRAM_H
#define PROGRAM_H

#include "gl.h"
#include "glm.h"
#include <string>
#include <vector>

typedef std::vector<std::string> string_vector;
typedef std::vector<const char*> cstring_vector;

namespace zxd {

// just a place hold, barely has any function.
struct program {
public:
  GLuint object;
  // some popular matrix
  glm::mat4 v_mat;
  glm::mat4 p_mat;
  glm::mat4 m_mat;
  glm::mat4 mv_mat;
  glm::mat4 vp_mat;
  glm::mat4 mvp_mat;

  glm::mat4 v_mat_i;
  glm::mat4 p_mat_i;
  glm::mat4 m_mat_i;
  glm::mat4 mv_mat_i;
  glm::mat4 vp_mat_i;
  glm::mat4 mvp_mat_i;

  glm::mat4 v_mat_t;
  glm::mat4 p_mat_t;
  glm::mat4 m_mat_t;
  glm::mat4 mv_mat_t;
  glm::mat4 vp_mat_t;
  glm::mat4 mvp_mat_t;

  glm::mat4 v_mat_it;
  glm::mat4 p_mat_it;
  glm::mat4 m_mat_it;
  glm::mat4 mv_mat_it;
  glm::mat4 vp_mat_it;
  glm::mat4 mvp_mat_it;

  // some popular location
  GLint ul_m_mat;
  GLint ul_v_mat;
  GLint ul_p_mat;
  GLint ul_mv_mat;
  GLint ul_vp_mat;
  GLint ul_mvp_mat;

  GLint ul_m_mat_i;
  GLint ul_v_mat_i;
  GLint ul_p_mat_i;
  GLint ul_mv_mat_i;
  GLint ul_vp_mat_i;
  GLint ul_mvp_mat_i;

  GLint ul_m_mat_t;
  GLint ul_v_mat_t;
  GLint ul_p_mat_t;
  GLint ul_mv_mat_t;
  GLint ul_vp_mat_t;
  GLint ul_mvp_mat_t;

  GLint ul_m_mat_it;
  GLint ul_v_mat_it;
  GLint ul_p_mat_it;
  GLint ul_mv_mat_it;
  GLint ul_vp_mat_it;
  GLint ul_mvp_mat_it;

protected:
  std::string m_name;

public:
  program() : object(-1) {}
  virtual ~program() {}

  operator GLuint() { return object; }

  virtual void init() {
    object = glCreateProgram();
    attach_shaders();
    link();
    bind_uniform_locations();
    bind_attrib_locations();
  }

  virtual void reload(){
    clear();
    init();
  }

  bool is_inited() { return object != -1; }

  void link();
  void use();
  
  void fix2d_camera(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top);

  virtual void attach_shaders(){};
  virtual void bind_uniform_locations(){};
  virtual void bind_attrib_locations(){};
  virtual void update_model(const glm::mat4& _m_mat) {}
  virtual void update_frame() {}

  GLint attrib_location(const std::string& name);
  void uniform_location(GLint* location, const std::string& name);

  void attach(GLenum type, const std::string& file);
  bool attach(GLenum type, const string_vector& source);
  void attach(
    GLenum type, const string_vector& source, const std::string& file);

  std::string name() const { return m_name; }
  void name(std::string v) { m_name = v; }

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
};
}

#endif /* PROGRAM_H */
