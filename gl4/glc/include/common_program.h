#ifndef GL_GLC_COMMON_PROGRAM_H
#define GL_GLC_COMMON_PROGRAM_H

#include "program.h"
#include "light.h"

namespace zxd
{

struct blinn_program : public zxd::program
{

  GLboolean with_texcoord;
  GLboolean instance;
  GLboolean legacy = GL_FALSE;

  GLint ul_map;
  GLint ul_normal_scale;
  GLint ul_mv_mat;
  GLint ul_mvp_mat;
  GLint ul_mv_mat_it;

  GLint tex_unit;
  GLint light_count = 1;

  std::string map_name;

  blinn_program()
      : with_texcoord(GL_FALSE),
        instance(GL_FALSE),
        map_name("diffuse_map"),
        tex_unit(0) {}

  void update_uniforms(const glm::mat4& m_mat,
    const glm::mat4& v_mat, const glm::mat4& p_mat);
  virtual void attach_shaders();
  virtual void bind_uniform_locations();

  // must be called before before you update lighting uniform
  virtual void bind_lighting_uniform_locations(
    light_vector& lights, light_model& lm, material& mtl);

  virtual void update_lighting_uniforms(light_vector& lights,
      light_model& lm, material& mtl, const glm::mat4& v_mat);

  virtual void bind_attrib_locations();
};

struct quad_program : public zxd::program
{
  GLint ul_quad_map;
  std::string quad_map_name;

  quad_program() : quad_map_name("quad_map") {}

  void attach_shaders();

  void update_uniforms(GLuint tex_index = 0);

  virtual void bind_uniform_locations();

  virtual void bind_attrib_locations();
};

struct point_program : public zxd::program
{

  GLint ul_mvp_mat;

  void attach_shaders();

  virtual void bind_uniform_locations();

  virtual void bind_attrib_locations();

  virtual void udpate_uniforms(const mat4& _mvp_mat);
};

struct normal_viewer_program : public zxd::program
{

  GLboolean smooth_normal = GL_FALSE;

  GLint ul_normal_length;
  GLint ul_color;
  GLint ul_mv_mat_it;
  GLint ul_mv_mat;
  GLint ul_p_mat;

  virtual void update_uniforms(const mat4& m_mat, const mat4& v_mat, const mat4& p_mat);
  virtual void attach_shaders();
  virtual void bind_uniform_locations();

  virtual void bind_attrib_locations();
};

struct vertex_color_program : public zxd::program
{

  GLint ul_mvp_mat;

  void attach_shaders();
  virtual void bind_uniform_locations();
  virtual void bind_attrib_locations();
  void update_uniforms(const mat4& _mvp_mat);
};

class texture_animation_program : public program
{

public:
  GLint al_vertex;
  GLint al_texcoord;
  GLint ul_diffuse_map;
  GLint ul_tex_mat;
  GLint ul_mvp_mat;

protected:

  void attach_shaders();
  void bind_uniform_locations();
  void bind_attrib_locations();

};

class lightless_program: public program
{
public:
  GLint ul_color;
  GLint ul_diffuse_map;
  GLint ul_mvp_mat;

  bool with_texcoord = false;
  bool with_color = false;
  bool instance = false;

protected:
  void attach_shaders();
  void bind_uniform_locations();
  void bind_attrib_locations();
};

class billboard_program: public program
{
public:
  GLint ul_camera_pos;
  GLint ul_camera_up;
  GLint ul_vp_mat;

  bool tex_offset = false;
  bool use_camera_up = true;
  bool scale = false;
  bool rotate = false;

public:
  void update_uniforms(const mat4& v_mat, const mat4& p_mat);

protected:
  void attach_shaders();
  void bind_uniform_locations();
  void bind_attrib_locations();

};

}

#endif /* GL_GLC_COMMON_PROGRAM_H */
