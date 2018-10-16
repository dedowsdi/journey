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
quad::quad(const vec3& corner/* = vec3(-0.5,-0.5,0)*/,
  const vec3& edge0_vec/* = vec3(1, 0, 0)*/,
  const vec3& edge1_vec/* = vec3(1, 0, 0)*/)
{
  setup(corner, edge0_vec, edge1_vec);
}

//--------------------------------------------------------------------
void quad::setup(const vec3& corner, const vec3& edge0_vec, const vec3& edge1_vec)
{
  m_v0 = corner;
  m_v1 = corner + edge0_vec;
  m_v2 = corner + edge0_vec + edge1_vec;
  m_v3 = corner + edge1_vec;
}

//--------------------------------------------------------------------
void quad::setup(GLfloat x0, GLfloat y0, GLfloat x1, GLfloat y1)
{
  m_v0 = vec3(x0, y0, 0);
  m_v1 = vec3(x1, y0, 0);
  m_v2 = vec3(x1, y1, 0);
  m_v3 = vec3(x0, y1, 0);
}

//--------------------------------------------------------------------
void quad::build_vertex() {
  vec3_array& vertices = *(new vec3_array());
  attrib_array(num_arrays(), array_ptr(&vertices));
  vertices.reserve(4);

  vertices.push_back(m_v0);
  vertices.push_back(m_v1);
  vertices.push_back(m_v2);
  vertices.push_back(m_v3);

  m_primitive_sets.clear();
  add_primitive_set(new draw_arrays(GL_TRIANGLE_FAN, 0, 4));
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

  texcoords.push_back(vec2{0.0f, 0.0f});
  texcoords.push_back(vec2{1.0f, 0.0f});
  texcoords.push_back(vec2{1.0f, 1.0f});
  texcoords.push_back(vec2{0.0f, 1.0f});
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
