#ifndef COMMON_PROGRAM_H
#define COMMON_PROGRAM_H

#include "program.h"
#include "light.h"

namespace zxd {

struct blinn_program : public zxd::program {
  // GLint ul_eye;

  GLint ul_map;
  GLint tex_unit;
  GLboolean with_texcoord;
  GLboolean instance;
  std::string map_name;

  blinn_program()
      : with_texcoord(GL_FALSE),
        instance(GL_FALSE),
        map_name("diffuse_map"),
        tex_unit(0) {}

  virtual void update_model(const glm::mat4& _m_mat);
  virtual void attach_shaders();
  virtual void bind_uniform_locations();

  // must be called before before you update lighting uniform
  virtual void bind_lighting_uniform_locations(
    light_vector& lights, light_model& lm, material& mtl);

  virtual void update_lighting_uniforms(
    light_vector& lights, light_model& lm, material& mtl);

  virtual void bind_attrib_locations();
};

struct quad_program : public zxd::program {
  GLint ul_quad_map;
  std::string quad_map_name;

  quad_program() : quad_map_name("quad_map") {}

  void attach_shaders();

  void update_uniforms(GLuint tex_index = 0);

  virtual void bind_uniform_locations();

  virtual void bind_attrib_locations();
};

struct quad_base : public zxd::program {
  GLint ul_quad_map;
  std::string quad_map_name;

  quad_base() : quad_map_name("quad_map") {}

  void attach_shaders();

  virtual void do_attach_shaders() {}

  void update_uniforms(GLuint tex_index = 0);

  virtual void do_update_uniforms() {}

  virtual void bind_uniform_locations();
  virtual void do_bind_uniform_locations() {}

  virtual void bind_attrib_locations();
  virtual void do_bind_attrib_locations() {}
};

struct point_program : public zxd::program {

  void attach_shaders();

  virtual void bind_uniform_locations();

  virtual void bind_attrib_locations();

  virtual void udpate_uniforms(const mat4& _mvp_mat);
};

struct normal_viewer_program : public zxd::program {

  GLboolean smooth_normal = GL_FALSE;
  GLint ul_normal_length;
  GLint ul_color;

  virtual void update_uniforms(const mat4& _m_mat, const mat4& _v_mat, const mat4& _p_mat);
  virtual void attach_shaders();
  virtual void bind_uniform_locations();

  virtual void bind_attrib_locations();
};

struct vertex_color_program : public zxd::program {

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

  bool tex_offset = false;
  bool use_camera_up = true;
  bool scale = false;
  bool rotate = false;

public:
  void update_uniforms();

protected:
  void attach_shaders();
  void bind_uniform_locations();
  void bind_attrib_locations();


};

}

#endif /* COMMON_PROGRAM_H */
