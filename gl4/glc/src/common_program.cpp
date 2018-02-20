#include "common_program.h"
#include "common.h"
#include <sstream>

namespace zxd {

//--------------------------------------------------------------------
void blinn_program::update_model(const glm::mat4& _m_mat) {
  // m_mat_i = glm::inverse(m_mat);
  m_mat = _m_mat;
  mv_mat = v_mat * m_mat;
  mv_mat_it = glm::inverse(glm::transpose(mv_mat));
  mvp_mat = p_mat * mv_mat;

  glUniformMatrix4fv(ul_mv_mat_it, 1, 0, value_ptr(mv_mat_it));
  glUniformMatrix4fv(ul_mv_mat, 1, 0, value_ptr(mv_mat));
  glUniformMatrix4fv(ul_mvp_mat, 1, 0, value_ptr(mvp_mat));
  if (with_texcoord) {
    glUniform1i(ul_map, tex_unit);
  }
}

//--------------------------------------------------------------------
void blinn_program::attach_shaders() {
  string_vector sv;
#ifdef GL_VERSION_3_3
  sv.push_back("#version 330 core\n");
#endif
  if (with_texcoord) sv.push_back("#define WITH_TEX\n");
  attach(GL_VERTEX_SHADER, sv, "data/shader/blinn.vs.glsl");

  sv.clear();

#ifdef GL_VERSION_3_3
  sv.push_back("#version 330 core\n");
#endif
  if (with_texcoord) sv.push_back("#define WITH_TEX\n");
  sv.push_back("#define LIGHT_COUNT 8\n");
  sv.push_back(read_file("data/shader/blinn.frag"));
  attach(GL_FRAGMENT_SHADER, sv, "data/shader/blinn.fs.glsl");
}

//--------------------------------------------------------------------
void blinn_program::bind_uniform_locations() {
  uniform_location(&ul_mv_mat, "mv_mat");
  uniform_location(&ul_mv_mat_it, "mv_mat_it");
  uniform_location(&ul_mvp_mat, "mvp_mat");
  if (with_texcoord) {
    uniform_location(&ul_map, map_name);
  }
}

//--------------------------------------------------------------------
void blinn_program::bind_lighting_uniform_locations(
  light_vector& lights, light_model& lm, material& mtl) {
  lm.bind_uniform_locations(object, "lm");
  for (int i = 0; i < lights.size(); ++i) {
    std::stringstream ss;
    ss << "lights[" << i << "]";
    lights[i].bind_uniform_locations(object, ss.str());
  }
  mtl.bind_uniform_locations(object, "mtl");
}

//--------------------------------------------------------------------
void blinn_program::update_lighting_uniforms(
  light_vector& lights, light_model& lm, material& mtl) {
  mtl.update_uniforms();
  lm.update_uniforms();
  for (int i = 0; i < lights.size(); ++i) {
    lights[i].update_uniforms(v_mat);
  }
}

//--------------------------------------------------------------------
void blinn_program::bind_attrib_locations() {
  al_vertex = attrib_location("vertex");
  al_normal = attrib_location("normal");
  if (with_texcoord) {
    al_texcoord = attrib_location("texcoord");
  }
}

//--------------------------------------------------------------------
void quad_program::attach_shaders() {
  attach(GL_VERTEX_SHADER, "data/shader/quad.vs.glsl");
  attach(GL_FRAGMENT_SHADER, "data/shader/quad.fs.glsl");
}

//--------------------------------------------------------------------
void quad_program::update_uniforms(GLuint tex_index /* = 0*/) {
  glUniform1i(ul_quad_map, tex_index);
}

//--------------------------------------------------------------------
void quad_program::bind_uniform_locations() {
  uniform_location(&ul_quad_map, quad_map_name);
}

//--------------------------------------------------------------------
void quad_program::bind_attrib_locations() {
  al_vertex = attrib_location("vertex");
  al_texcoord = attrib_location("texcoord");
}

//--------------------------------------------------------------------
void quad_base::attach_shaders() {
  attach(GL_VERTEX_SHADER, "data/shader/quad.vs.glsl");
  do_attach_shaders();
}

//--------------------------------------------------------------------
void quad_base::update_uniforms(GLuint tex_index /* = 0*/) {
  glUniform1i(ul_quad_map, tex_index);
  do_update_uniforms();
}

//--------------------------------------------------------------------
void quad_base::bind_uniform_locations() {
  uniform_location(&ul_quad_map, quad_map_name);
  do_bind_uniform_locations();
}

//--------------------------------------------------------------------
void quad_base::bind_attrib_locations() {
  al_vertex = attrib_location("vertex");
  al_texcoord = attrib_location("texcoord");
  do_bind_attrib_locations();
}

//--------------------------------------------------------------------
void point_program::attach_shaders() {
  attach(GL_VERTEX_SHADER, "data/shader/simple.vs.glsl");
  attach(GL_FRAGMENT_SHADER, "data/shader/color.fs.glsl");
}

//--------------------------------------------------------------------
void point_program::bind_uniform_locations() {
  uniform_location(&ul_mvp_mat, "mvp_mat");
}

//--------------------------------------------------------------------
void point_program::bind_attrib_locations() {
  al_vertex = attrib_location("vertex");
}

//--------------------------------------------------------------------
void point_program::udpate_uniforms(const mat4& _mvp_mat) {
  glUniformMatrix4fv(ul_mvp_mat, 1, GL_FALSE, value_ptr(_mvp_mat));
}

//--------------------------------------------------------------------
void normal_viewer_program::update_model(const mat4& _m_mat) {
  m_mat = _m_mat;
  mv_mat = v_mat * m_mat;
  mv_mat_it = glm::inverse(glm::transpose(mv_mat));

  glUniformMatrix4fv(ul_mv_mat_it, 1, 0, value_ptr(mv_mat_it));
  glUniformMatrix4fv(ul_mv_mat, 1, 0, value_ptr(mv_mat));
  glUniformMatrix4fv(ul_p_mat, 1, 0, value_ptr(p_mat));
}

//--------------------------------------------------------------------
void normal_viewer_program::attach_shaders() {
  attach(GL_VERTEX_SHADER, "data/shader/normal_viewer.vs.glsl");
  attach(GL_GEOMETRY_SHADER, "data/shader/normal_viewer.gs.glsl");
  attach(GL_FRAGMENT_SHADER, "data/shader/color.fs.glsl");
}

//--------------------------------------------------------------------
void normal_viewer_program::bind_uniform_locations() {
  uniform_location(&ul_p_mat, "p_mat");
  uniform_location(&ul_mv_mat, "mv_mat");
  uniform_location(&ul_mv_mat_it, "mv_mat_it");

  uniform_location(&ul_normal_length, "normal_length");
  uniform_location(&ul_color, "color");
}

//--------------------------------------------------------------------
void normal_viewer_program::bind_attrib_locations() {
  al_vertex = attrib_location("vertex");
  al_normal = attrib_location("normal");
}

//--------------------------------------------------------------------
void vertex_color_program::attach_shaders() {
  attach(GL_VERTEX_SHADER, "data/shader/vertex_color.vs.glsl");
  attach(GL_FRAGMENT_SHADER, "data/shader/vertex_color.fs.glsl");
}

//--------------------------------------------------------------------
void vertex_color_program::bind_uniform_locations() {
  uniform_location(&ul_mvp_mat, "mvp_mat");
}

//--------------------------------------------------------------------
void vertex_color_program::bind_attrib_locations() {
  al_vertex = attrib_location("vertex");
  al_color = attrib_location("color");
}

//--------------------------------------------------------------------
void vertex_color_program::update_uniforms(const mat4& _mvp_mat) {
  glUniformMatrix4fv(ul_mvp_mat, 1, 0, value_ptr(_mvp_mat));
}
}
