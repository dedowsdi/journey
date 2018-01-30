#ifndef PROGRAM_H
#define PROGRAM_H

#include "glad/glad.h"
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

  void link();

  virtual void attach_shaders(){};
  virtual void bind_uniform_locations(){};
  virtual void bind_attrib_locations(){};

  GLint attrib_location(const std::string& name);

  // some wrapper
  void uniform_location(GLint* location, const std::string& name);
  void attach_shader_file(GLenum type, const std::string& file);
  bool attach_shader_source(GLenum type, const string_vector& source);
  void attach_shader_source_and_file(
    GLenum type, const string_vector& source, const std::string& file);

  std::string name() const { return m_name; }
  void set_name(std::string v) { m_name = v; }

  void clear();
};
}

#endif /* PROGRAM_H */
