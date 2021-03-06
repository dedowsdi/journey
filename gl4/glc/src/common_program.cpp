#include <common_program.h>

#include <sstream>
#include <glm/gtc/matrix_access.hpp>

#include <common.h>
#include <stream_util.h>

namespace zxd
{

//--------------------------------------------------------------------
void blinn_program::update_uniforms(const glm::mat4& m_mat,
    const glm::mat4& v_mat, const glm::mat4& p_mat)
{

  mat4 mv_mat = v_mat * m_mat;
  mat4 mv_mat_it = glm::inverse(glm::transpose(mv_mat));
  mat4 mvp_mat = p_mat * mv_mat;

  glUniformMatrix4fv(ul_mv_mat_it, 1, 0, value_ptr(mv_mat_it));
  glUniformMatrix4fv(ul_mv_mat, 1, 0, value_ptr(mv_mat));
  glUniformMatrix4fv(ul_mvp_mat, 1, 0, value_ptr(mvp_mat));
  if (with_texcoord)
  {
    glUniform1i(ul_map, tex_unit);
  }
}

//--------------------------------------------------------------------
void blinn_program::attach_shaders()
{
  string_vector sv;
  std::string shader_dir = legacy ? "shader2/" : "shader4/";
#ifdef GL_VERSION_3_3
  sv.push_back("#version 330 core\n");
#endif
  if (with_texcoord) sv.push_back("#define WITH_TEX\n");
  if (instance) sv.push_back("#define INSTANCE\n");
  if (with_clipplane0) sv.push_back("#define WITH_CLIPPLANE0\n");
  attach(GL_VERTEX_SHADER, sv, shader_dir + "blinn.vs.glsl");

  sv.clear();

#ifdef GL_VERSION_3_3
  sv.push_back("#version 330 core\n");
#endif
  if (with_texcoord) sv.push_back("#define WITH_TEX\n");
  sv.push_back("#define LIGHT_COUNT " + std::to_string(light_count) + "\n");
  sv.push_back(stream_util::read_resource(shader_dir + "blinn.frag"));
  attach(GL_FRAGMENT_SHADER, sv, shader_dir + "blinn.fs.glsl");
}

//--------------------------------------------------------------------
void blinn_program::bind_uniform_locations()
{
  if(!this->instance)
  {
    ul_mv_mat = get_uniform_location("mv_mat");
    ul_mv_mat_it = get_uniform_location("mv_mat_it");
    ul_mvp_mat = get_uniform_location("mvp_mat");
  }
  if (with_texcoord)
  {
    ul_map = get_uniform_location(map_name);
  }
  if (with_clipplane0)
  {
    ul_clipplane0 = get_uniform_location("clipplane0");
  }
  ul_normal_scale = get_uniform_location("normal_scale");
}

//--------------------------------------------------------------------
void blinn_program::bind_lighting_uniform_locations(
  light_vector& lights, light_model& lm, material& mtl)
{
  lm.bind_uniform_locations(get_object(), "lm");
  for (int i = 0; i < lights.size(); ++i)
  {
    std::stringstream ss;
    ss << "lights[" << i << "]";
    lights[i].bind_uniform_locations(get_object(), ss.str());
  }
  mtl.bind_uniform_locations(get_object(), "mtl");
}

//--------------------------------------------------------------------
void blinn_program::update_lighting_uniforms( light_vector& lights,
    light_model& lm, material& mtl, const glm::mat4& v_mat)
{
  mtl.update_uniforms();
  lm.update_uniforms();
  for (int i = 0; i < lights.size(); ++i)
  {
    lights[i].update_uniforms(v_mat);
  }
}

//--------------------------------------------------------------------
void blinn_program::bind_attrib_locations()
{
  bind_attrib_location(0, "vertex");
  bind_attrib_location(1, "normal");
  if (with_texcoord)
  {
    bind_attrib_location(2, "texcoord");
  }

  if(instance)
  {
    bind_attrib_location(3, "mv_mat");
    bind_attrib_location(7, "mvp_mat");
    bind_attrib_location(11, "mv_mat_it");
  }
}

//--------------------------------------------------------------------
void quad_program::attach_shaders()
{
  attach(GL_VERTEX_SHADER, "shader4/quad.vs.glsl");
  attach(GL_FRAGMENT_SHADER, frag_shader);
}

//--------------------------------------------------------------------
void quad_program::update_uniforms(GLuint tex_index /* = 0*/)
{
  glUniform1i(ul_quad_map, tex_index);
}

//--------------------------------------------------------------------
void quad_program::bind_uniform_locations()
{
  ul_quad_map = get_uniform_location(quad_map_name);
}

//--------------------------------------------------------------------
void quad_program::bind_attrib_locations()
{
  bind_attrib_location(0, "vertex");
  bind_attrib_location(1, "texcoord");
}

//--------------------------------------------------------------------
void point_program::attach_shaders()
{
  attach(GL_VERTEX_SHADER, "shader4/simple.vs.glsl");
  attach(GL_FRAGMENT_SHADER, "shader4/color.fs.glsl");
}

//--------------------------------------------------------------------
void point_program::bind_uniform_locations()
{
  ul_mvp_mat = get_uniform_location("mvp_mat");
}

//--------------------------------------------------------------------
void point_program::bind_attrib_locations()
{
  bind_attrib_location(0, "vertex");
}

//--------------------------------------------------------------------
void point_program::udpate_uniforms(const mat4& _mvp_mat)
{
  glUniformMatrix4fv(ul_mvp_mat, 1, GL_FALSE, value_ptr(_mvp_mat));
}

//--------------------------------------------------------------------
void normal_viewer_program::update_uniforms(const mat4& m_mat, const mat4& v_mat, const mat4& p_mat)
{
  mat4 mv_mat = v_mat * m_mat;
  mat4 mv_mat_it = glm::inverse(glm::transpose(mv_mat));

  glUniformMatrix4fv(ul_mv_mat_it, 1, 0, value_ptr(mv_mat_it));
  glUniformMatrix4fv(ul_mv_mat, 1, 0, value_ptr(mv_mat));
  glUniformMatrix4fv(ul_p_mat, 1, 0, value_ptr(p_mat));
}

//--------------------------------------------------------------------
void normal_viewer_program::attach_shaders()
{
  string_vector sv;
#ifdef GL_VERSION_3_3
  sv.push_back("#version 330 core\n");
#endif

  if (smooth_normal)
    sv.push_back("#define SMOOTH_NORMAL\n");

  attach(GL_VERTEX_SHADER, "shader4/normal_viewer.vs.glsl");
#ifdef GL_VERSION_3_3
  attach(GL_GEOMETRY_SHADER, sv, "shader4/normal_viewer.gs.glsl");
#else
  attach(GL_GEOMETRY_SHADER_ARB, sv, "shader4/normal_viewer.gs.glsl");
#endif
  attach(GL_FRAGMENT_SHADER, "shader4/color.fs.glsl");
}

//--------------------------------------------------------------------
void normal_viewer_program::bind_uniform_locations()
{
  ul_p_mat = get_uniform_location("p_mat");
  ul_mv_mat = get_uniform_location("mv_mat");
  ul_mv_mat_it = get_uniform_location("mv_mat_it");

  ul_normal_length = get_uniform_location("normal_length");
  ul_color = get_uniform_location("color");
}

//--------------------------------------------------------------------
void normal_viewer_program::bind_attrib_locations()
{
  bind_attrib_location(0, "vertex");
  bind_attrib_location(1, "normal");
}

//--------------------------------------------------------------------
void vertex_color_program::attach_shaders()
{
  attach(GL_VERTEX_SHADER, "shader4/vertex_color.vs.glsl");
  attach(GL_FRAGMENT_SHADER, "shader4/vertex_color.fs.glsl");
}

//--------------------------------------------------------------------
void vertex_color_program::bind_uniform_locations()
{
  ul_mvp_mat = get_uniform_location("mvp_mat");
}

//--------------------------------------------------------------------
void vertex_color_program::bind_attrib_locations()
{
  bind_attrib_location(0, "vertex");
  bind_attrib_location(1, "color");
}

//--------------------------------------------------------------------
void vertex_color_program::update_uniforms(const mat4& _mvp_mat)
{
  glUniformMatrix4fv(ul_mvp_mat, 1, 0, value_ptr(_mvp_mat));
}

//--------------------------------------------------------------------
void texture_animation_program::attach_shaders()
{
  attach(GL_VERTEX_SHADER, "shader4/texture_animation.vs.glsl");
  attach(GL_FRAGMENT_SHADER, "shader4/texture_animation.fs.glsl");
}

//--------------------------------------------------------------------
void texture_animation_program::bind_uniform_locations()
{
  ul_mvp_mat = get_uniform_location("mvp_mat");
  ul_diffuse_map = get_uniform_location("diffuse_map");
  ul_tex_mat = get_uniform_location("tex_mat");
}

//--------------------------------------------------------------------
void texture_animation_program::bind_attrib_locations()
{
  bind_attrib_location(0, "vertex");
  bind_attrib_location(1, "texcoord");
}

//--------------------------------------------------------------------
void lightless_program::attach_shaders()
{
  if(with_texcoord && with_color)
    throw std::runtime_error("you should not include tex and color at the same time");

  string_vector sv;
  sv.push_back("#version 330 core\n");
  if(with_texcoord)
  {
    sv.push_back("#define WITH_TEXCOORD\n");
  }
  if(instance)
  {
    sv.push_back("#define INSTANCE\n");
  }
  if(with_color)
  {
    sv.push_back("#define WITH_COLOR\n");
  }

  attach(GL_VERTEX_SHADER, sv, "shader4/lightless.vs.glsl");
  attach(GL_FRAGMENT_SHADER, sv, "shader4/lightless.fs.glsl");
}

//--------------------------------------------------------------------
void lightless_program::bind_uniform_locations()
{
  ul_mvp_mat = get_uniform_location("mvp_mat");
  if(with_texcoord)
  {
    ul_diffuse_map = get_uniform_location("diffuse_map");
  }
  else if(!with_color)
  {
    ul_color = get_uniform_location("color");
  }
}

//--------------------------------------------------------------------
void lightless_program::bind_attrib_locations()
{
  bind_attrib_location(0, "vertex");
  if(with_texcoord)
    bind_attrib_location(1, "texcoord");
  if(with_color)
    bind_attrib_location(1, "color");
  if(instance)
    bind_attrib_location(2, "m_mat");
}

//--------------------------------------------------------------------
void billboard_program::attach_shaders()
{
  string_vector sv;
  sv.push_back("#version 430 core\n");
  if(use_camera_up)
    sv.push_back("#define USE_CAMERA_UP\n");
  if(scale)
    sv.push_back("#define SCALE\n");
  if(rotate)
    sv.push_back("#define ROTATE\n");
  if(tex_offset)
    sv.push_back("#define TEX_OFFSET\n");

  attach(GL_VERTEX_SHADER, sv, "shader4/billboard.vs.glsl");
  attach(GL_FRAGMENT_SHADER, sv, "shader4/billboard.fs.glsl");
}

//--------------------------------------------------------------------
void billboard_program::bind_uniform_locations()
{
  ul_vp_mat = get_uniform_location("vp_mat");
  ul_camera_pos = get_uniform_location("camera_pos");
  if(use_camera_up)
    ul_camera_up = get_uniform_location("camera_up");
}

//--------------------------------------------------------------------
void billboard_program::bind_attrib_locations()
{
  // It is permissible to bind a generic attribute index to an attribute
  // variable name that is never used in a vertex shader.
  bind_attrib_location(0, "vertex");
  bind_attrib_location(1, "texcoord");
  bind_attrib_location(2, "translation");
  bind_attrib_location(3, "size");
  bind_attrib_location(4, "angle");
}

//--------------------------------------------------------------------
void billboard_program::update_uniforms(const mat4& v_mat, const mat4& p_mat)
{
  vec3 camera_pos = vec3(-v_mat[3]);
  camera_pos = vec3(glm::dot(vec3(v_mat[0]), camera_pos),
      glm::dot(vec3(v_mat[1]), camera_pos),
      glm::dot(vec3(v_mat[2]), camera_pos));
  glUniform3fv(ul_camera_pos, 1,  glm::value_ptr(camera_pos));

  if(use_camera_up)
    glUniform3fv(ul_camera_up, 1,  glm::value_ptr(vec3(glm::row(v_mat, 1))));

  glUniformMatrix4fv(ul_vp_mat, 1, 0, glm::value_ptr(p_mat * v_mat));
}

}
