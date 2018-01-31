#ifndef QUAD_H
#define QUAD_H
#include "program.h"
#include "glm.h"
#include <array>
#include "common.h"
#include "geometry.h"

namespace zxd {

struct quad_program : public zxd::program {
  GLint ul_quad_map;
  GLint al_vertex;
  GLint al_texcoord;

  quad_program() {}

  void attach_shaders() {
    attach_shader_file(GL_VERTEX_SHADER, "data/shader/quad.vs.glsl");
    attach_shader_file(GL_FRAGMENT_SHADER, "data/shader/quad.fs.glsl");
  }

  void update_uniforms(GLuint tex_index = 0) {
    glUniform1i(ul_quad_map, tex_index);
  }

  virtual void bind_uniform_locations() {
    uniform_location(&ul_quad_map, "quad_map");
  }

  virtual void bind_attrib_locations() {
    al_vertex = attrib_location("vertex");
    al_texcoord = attrib_location("texcoord");
  }
};

// draw single 2d quad
void draw_quad(GLuint tex_index = 0);

class quad : public geometry {
public:
  quad(const glm::vec3& half_diag) {}
  quad(GLfloat size = 1) {}

  void build_vertex();
  void build_normal();
  void build_texcoord();
  void build_tangent();

  void draw(GLuint primcount = 1);
};
}

#endif /* QUAD_H */
