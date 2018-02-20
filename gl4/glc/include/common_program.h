#ifndef COMMON_PROGRAM_H
#define COMMON_PROGRAM_H

#include "program.h"
#include "light.h"

namespace zxd {

struct blinn_program : public zxd::program {
  // GLint ul_eye;
  GLint al_vertex;
  GLint al_normal;
  GLint al_texcoord;

  GLint ul_map;
  GLint tex_unit;
  GLboolean with_texcoord;
  std::string map_name;

  blinn_program()
      : al_vertex(-1),
        al_normal(-1),
        al_texcoord(-1),
        with_texcoord(GL_FALSE),
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
  GLint al_vertex;
  GLint al_texcoord;
  std::string quad_map_name;

  quad_program() : quad_map_name("quad_map") {}

  void attach_shaders();

  void update_uniforms(GLuint tex_index = 0);

  virtual void bind_uniform_locations();

  virtual void bind_attrib_locations();
};

struct quad_base : public zxd::program {
  GLint ul_quad_map;
  GLint al_vertex;
  GLint al_texcoord;
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
  GLint al_vertex;

  void attach_shaders();

  virtual void bind_uniform_locations();

  virtual void bind_attrib_locations();

  virtual void udpate_uniforms(const mat4& _mvp_mat);
};

struct normal_viewer_program : public zxd::program {
  GLint al_vertex;
  GLint al_normal;

  GLint ul_normal_length;
  GLint ul_color;

  virtual void update_model(const mat4& _m_mat);
  virtual void attach_shaders();
  virtual void bind_uniform_locations();

  virtual void bind_attrib_locations();
};

struct vertex_color_program : public zxd::program {
  GLint al_vertex;
  GLint al_color;

  void attach_shaders();
  virtual void bind_uniform_locations();
  virtual void bind_attrib_locations();
  void update_uniforms(const mat4& _mvp_mat);
};
}

#endif /* COMMON_PROGRAM_H */
