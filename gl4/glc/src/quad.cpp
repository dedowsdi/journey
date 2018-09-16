#include "quad.h"
#include "common.h"
#include "common_program.h"

namespace zxd {

//--------------------------------------------------------------------
void draw_quad(GLuint tex_index /* = 0*/) {
  static quad q;
  static quad_program prg;
  q.build_mesh();

  if (prg.object == -1) {
    prg.init();
    q.bind(prg.al_vertex, prg.al_texcoord);
  }

  glUseProgram(prg.object);
  prg.update_uniforms(tex_index);
  q.draw();
}

//--------------------------------------------------------------------
GLint quad::build_vertex() {
  vec2_array& vertices = *(new vec2_array());
  attrib_array(num_arrays(), array_ptr(&vertices));
  vertices.reserve(4);

  GLfloat hz = m_size * 0.5;
  vertices.push_back(vec2(-hz, hz));
  vertices.push_back(vec2(-hz, -hz));
  vertices.push_back(vec2(hz, hz));
  vertices.push_back(vec2(hz, -hz));
  return num_arrays() - 1;
}

//--------------------------------------------------------------------
GLint quad::build_normal() {
  vec3_array& normals = *(new vec3_array());
  attrib_array(num_arrays(), array_ptr(&normals));
  normals.reserve(4);

  normals.push_back(vec3{0.0f, 0.0f, 1.0f});
  normals.push_back(vec3{0.0f, 0.0f, 1.0f});
  normals.push_back(vec3{0.0f, 0.0f, 1.0f});
  normals.push_back(vec3{0.0f, 0.0f, 1.0f});
  return num_arrays() - 1;
}

//--------------------------------------------------------------------
GLint quad::build_texcoord() {
  vec2_array& texcoords = *(new vec2_array());
  attrib_array(num_arrays(), array_ptr(&texcoords));
  texcoords.reserve(4);

  texcoords.push_back(vec2{0.0f, 1.0f});
  texcoords.push_back(vec2{0.0f, 0.0f});
  texcoords.push_back(vec2{1.0f, 1.0f});
  texcoords.push_back(vec2{1.0f, 0.0f});
  return num_arrays() - 1;
}

//--------------------------------------------------------------------
GLint quad::build_tangent() {
  vec3_array& tangents = *(new vec3_array());
  attrib_array(num_arrays(), array_ptr(&tangents));
  tangents.reserve(4);

  tangents.push_back(vec3{1.0f, 0.0f, 0.0f});
  tangents.push_back(vec3{1.0f, 0.0f, 0.0f});
  tangents.push_back(vec3{1.0f, 0.0f, 0.0f});
  tangents.push_back(vec3{1.0f, 0.0f, 0.0f});
  return num_arrays() - 1;
}

//--------------------------------------------------------------------
void quad::draw(GLuint primcount /* = 1*/) {
  bind_vao();

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
