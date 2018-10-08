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
  }

  glUseProgram(prg.object);
  prg.update_uniforms(tex_index);
  q.draw();
}

//--------------------------------------------------------------------
void quad::build_vertex() {
  vec2_array& vertices = *(new vec2_array());
  attrib_array(num_arrays(), array_ptr(&vertices));
  vertices.reserve(4);

  GLfloat hw = m_width * 0.5;
  GLfloat hh = m_height * 0.5;
  vertices.push_back(vec2(-hw, hh));
  vertices.push_back(vec2(-hw, -hh));
  vertices.push_back(vec2(hw, hh));
  vertices.push_back(vec2(hw, -hh));

  m_primitive_sets.clear();
  add_primitive_set(new draw_arrays(GL_TRIANGLE_STRIP, 0, 4));
}

//--------------------------------------------------------------------
void quad::build_normal() {
  vec3_array& normals = *(new vec3_array());
  attrib_array(num_arrays(), array_ptr(&normals));
  normals.reserve(4);

  normals.push_back(vec3{0.0f, 0.0f, 1.0f});
  normals.push_back(vec3{0.0f, 0.0f, 1.0f});
  normals.push_back(vec3{0.0f, 0.0f, 1.0f});
  normals.push_back(vec3{0.0f, 0.0f, 1.0f});
}

//--------------------------------------------------------------------
void quad::build_texcoord() {
  vec2_array& texcoords = *(new vec2_array());
  attrib_array(num_arrays(), array_ptr(&texcoords));
  texcoords.reserve(4);

  texcoords.push_back(vec2{0.0f, 1.0f});
  texcoords.push_back(vec2{0.0f, 0.0f});
  texcoords.push_back(vec2{1.0f, 1.0f});
  texcoords.push_back(vec2{1.0f, 0.0f});
}

//--------------------------------------------------------------------
void quad::build_tangent() {
  vec3_array& tangents = *(new vec3_array());
  attrib_array(num_arrays(), array_ptr(&tangents));
  tangents.reserve(4);

  tangents.push_back(vec3{1.0f, 0.0f, 0.0f});
  tangents.push_back(vec3{1.0f, 0.0f, 0.0f});
  tangents.push_back(vec3{1.0f, 0.0f, 0.0f});
  tangents.push_back(vec3{1.0f, 0.0f, 0.0f});
}

}
