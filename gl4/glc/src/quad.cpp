#include "quad.h"
#include "common.h"
#include "common_program.h"

namespace zxd {

//--------------------------------------------------------------------
void draw_quad(GLuint tex_index /* = 0*/) {
  static quad q;
  static quad_program prg;
  q.build_mesh(1, 1, 1);

  if (prg.object == -1) {
    prg.init();
    q.bind(prg.al_vertex, prg.al_texcoord);
  }

  glUseProgram(prg.object);
  prg.update_uniforms(tex_index);
  q.draw();
}

//--------------------------------------------------------------------
void quad::build_vertex() {
  m_vertices.clear();
  m_vertices.reserve(4);

  m_vertices.push_back(glm::vec2(-1.0f, 1.0f));
  m_vertices.push_back(glm::vec2(-1.0f, -1.0f));
  m_vertices.push_back(glm::vec2(1.0f, 1.0f));
  m_vertices.push_back(glm::vec2(1.0f, -1.0f));
}

//--------------------------------------------------------------------
void quad::build_normal() {
  m_normals.clear();
  m_normals.reserve(4);

  m_normals.push_back(glm::vec3{0.0f, 0.0f, 1.0f});
  m_normals.push_back(glm::vec3{0.0f, 0.0f, 1.0f});
  m_normals.push_back(glm::vec3{0.0f, 0.0f, 1.0f});
  m_normals.push_back(glm::vec3{0.0f, 0.0f, 1.0f});
}

//--------------------------------------------------------------------
void quad::build_texcoord() {
  m_texcoords.clear();
  m_texcoords.reserve(4);

  m_texcoords.push_back(glm::vec2{0.0f, 1.0f});
  m_texcoords.push_back(glm::vec2{0.0f, 0.0f});
  m_texcoords.push_back(glm::vec2{1.0f, 1.0f});
  m_texcoords.push_back(glm::vec2{1.0f, 0.0f});
}

//--------------------------------------------------------------------
void quad::build_tangent() {
  m_tangents.clear();
  m_tangents.reserve(4);

  m_tangents.push_back(glm::vec3{1.0f, 0.0f, 0.0f});
  m_tangents.push_back(glm::vec3{1.0f, 0.0f, 0.0f});
  m_tangents.push_back(glm::vec3{1.0f, 0.0f, 0.0f});
  m_tangents.push_back(glm::vec3{1.0f, 0.0f, 0.0f});
}

//--------------------------------------------------------------------
void quad::draw(GLuint primcount /* = 1*/) {
  bind_vertex_array_object();

  if (primcount == 1) {
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  } else {
#ifdef GL_VERSION_3_0
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, primcount);
#else
    glDrawArraysInstancedARB(GL_TRIANGLE_STRIP, 0, 4, primcount);
#endif
  }
}
}
