#ifndef COMMON_PROGRAM_H
#define COMMON_PROGRAM_H

#include "program.h"
#include "light.h"
#include <sstream>

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

  virtual void update_model(const glm::mat4& _m_mat) {
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
  virtual void attach_shaders() {
    string_vector sv;
    if (with_texcoord) sv.push_back("#define WITH_TEX\n");
    attach(
      GL_VERTEX_SHADER, sv, "data/shader/blinn.vs.glsl");

    sv.clear();
    if (with_texcoord) sv.push_back("#define WITH_TEX\n");
    sv.push_back("#define LIGHT_COUNT 8\n");
    sv.push_back(read_file("data/shader/blinn.frag"));
    attach(
      GL_FRAGMENT_SHADER, sv, "data/shader/blinn.fs.glsl");
  }
  virtual void bind_uniform_locations() {
    uniform_location(&ul_mv_mat, "mv_mat");
    uniform_location(&ul_mv_mat_it, "mv_mat_it");
    uniform_location(&ul_mvp_mat, "mvp_mat");
    if (with_texcoord) {
      uniform_location(&ul_map, map_name);
    }
  }

  // must be called before before you update lighting uniform
  virtual void bind_lighting_uniform_locations(
    light_vector& lights, light_model& lm, material& mtl) {
    lm.bind_uniform_locations(object, "lm");
    for (int i = 0; i < lights.size(); ++i) {
      std::stringstream ss;
      ss << "lights[" << i << "]";
      lights[i].bind_uniform_locations(object, ss.str());
    }
    mtl.bind_uniform_locations(object, "mtl");
  }

  virtual void update_lighting_uniforms(
    light_vector& lights, light_model& lm, material& mtl) {
    mtl.update_uniforms();
    lm.update_uniforms();
    for (int i = 0; i < lights.size(); ++i) {
      lights[i].update_uniforms(v_mat);
    }
  }

  virtual void bind_attrib_locations() {
    al_vertex = attrib_location("vertex");
    al_normal = attrib_location("normal");
    if (with_texcoord) {
      al_texcoord = attrib_location("texcoord");
    }
  }
};

struct quad_program : public zxd::program {
  GLint ul_quad_map;
  GLint al_vertex;
  GLint al_texcoord;
  std::string quad_map_name;

  quad_program() : quad_map_name("quad_map") {}

  void attach_shaders() {
    attach(GL_VERTEX_SHADER, "data/shader/quad.vs.glsl");
    attach(GL_FRAGMENT_SHADER, "data/shader/quad.fs.glsl");
  }

  void update_uniforms(GLuint tex_index = 0) {
    glUniform1i(ul_quad_map, tex_index);
  }

  virtual void bind_uniform_locations() {
    uniform_location(&ul_quad_map, quad_map_name);
  }

  virtual void bind_attrib_locations() {
    al_vertex = attrib_location("vertex");
    al_texcoord = attrib_location("texcoord");
  }
};

struct quad_base : public zxd::program {
  GLint ul_quad_map;
  GLint al_vertex;
  GLint al_texcoord;
  std::string quad_map_name;

  quad_base() : quad_map_name("quad_map") {}

  void attach_shaders() {
    attach(GL_VERTEX_SHADER, "data/shader/quad.vs.glsl");
    do_attach_shaders();
  }

  virtual void do_attach_shaders() {}

  void update_uniforms(GLuint tex_index = 0) {
    glUniform1i(ul_quad_map, tex_index);
    do_update_uniforms();
  }

  virtual void do_update_uniforms() {}

  virtual void bind_uniform_locations() {
    uniform_location(&ul_quad_map, quad_map_name);
    do_bind_uniform_locations();
  }
  virtual void do_bind_uniform_locations() {}

  virtual void bind_attrib_locations() {
    al_vertex = attrib_location("vertex");
    al_texcoord = attrib_location("texcoord");
    do_bind_attrib_locations();
  }
  virtual void do_bind_attrib_locations() {}
};
}

#endif /* COMMON_PROGRAM_H */
