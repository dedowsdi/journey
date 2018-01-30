#ifndef QUAD_H
#define QUAD_H
#include "program.h"
#include "glm.h"
#include <array>
#include "common.h"

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

void draw_quad(GLuint texture);

// triangle strip quad
struct quad {
  GLuint vao;
  // clang-format off
  std::array<glm::vec2, 4> vertices{
    glm::vec2(-1.0f, 1.0f),
    glm::vec2(-1.0f, -1.0f),
    glm::vec2(1.0f, 1.0f),
    glm::vec2(1.0f, -1.0f) 
  };
  std::array<glm::vec2, 4> texcoords{
    glm::vec2{0.0f, 1.0f},
    glm::vec2{0.0f, 0.0f}, 
    glm::vec2{1.0f, 1.0f}, 
    glm::vec2{1.0f, 0.0f}
  };
  std::array<glm::vec3, 4> normals{
    glm::vec3{0.0f, 0.0f, 1.0f}, 
    glm::vec3{0.0f, 0.0f, 1.0f}, 
    glm::vec3{0.0f, 0.0f, 1.0f},
    glm::vec3{0.0f, 0.0f, 1.0f}
  };
  std::array<glm::vec3, 4> tangents{
    glm::vec3{1.0f, 0.0f, 0.0f }, 
    glm::vec3{1.0f, 0.0f, 0.0f }, 
    glm::vec3{1.0f, 0.0f, 0.0f },
    glm::vec3{1.0f, 0.0f, 0.0f }
  };
  // clang-format on

  void draw() {
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  }

  void setup_vertex_attrib(GLint al_vertex, GLint al_texcoord = -1,
    GLint al_normal = -1, GLint al_tangent = -1) {
    if (!glIsVertexArray(vao)) glGenVertexArrays(1, &vao);

    glBindVertexArray(vao);
    if (al_vertex != -1) zxd::setup_vertex_attrib(al_vertex, vertices);
    if (al_texcoord != -1) zxd::setup_vertex_attrib(al_texcoord, texcoords);
    if (al_normal != -1) zxd::setup_vertex_attrib(al_normal, normals);
    if (al_tangent != -1) zxd::setup_vertex_attrib(al_tangent, tangents);
  }
};
}

#endif /* QUAD_H */
