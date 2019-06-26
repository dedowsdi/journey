#include "quad.h"

#include "common.h"
#include "common_program.h"

namespace zxd
{

namespace
{
quad ndc_quad;
}

quad& get_ndc_quad()
{
  if(!ndc_quad.is_inited())
  {
    ndc_quad.include_texcoord(true);
    ndc_quad.build_mesh();
  }
  return ndc_quad;
}

//--------------------------------------------------------------------
void draw_quad(GLuint tex, GLuint tui/* = 0*/)
{
  static quad q;
  static quad_program prg;
  if (q.vao() == -1)
  {
    q.include_texcoord(true);
    q.build_mesh();
  }

  glActiveTexture(tui + GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, tex);

  if (prg.object == -1)
  {
    prg.init();
  }

  prg.use();
  prg.update_uniforms(tui);
  q.draw();
}

//--------------------------------------------------------------------
void draw_quad()
{
  get_ndc_quad().draw();
}

//--------------------------------------------------------------------
quad::quad(const vec3& corner/* = vec3(-0.5,-0.5,0)*/,
  const vec3& edge0_vec/* = vec3(1, 0, 0)*/,
  const vec3& edge1_vec/* = vec3(1, 0, 0)*/)
{
  setup(corner, edge0_vec, edge1_vec);
}

//--------------------------------------------------------------------
void quad::setup(const vec3& corner, const vec3& edge0_vec, const vec3& edge1_vec,
  const vec2& tc0 , const vec2& tc1)
{
  m_v0 = corner;
  m_v1 = corner + edge0_vec;
  m_v2 = corner + edge0_vec + edge1_vec;
  m_v3 = corner + edge1_vec;
  m_tc0 = tc0;
  m_tc1 = tc1;
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
void quad::build_vertex()
{
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
void quad::build_normal()
{
  vec3_array& normals = *(new vec3_array());
  attrib_array(num_arrays(), array_ptr(&normals));
  normals.reserve(4);

  normals.push_back(vec3{0.0f, 0.0f, 1.0f});
  normals.push_back(vec3{0.0f, 0.0f, 1.0f});
  normals.push_back(vec3{0.0f, 0.0f, 1.0f});
  normals.push_back(vec3{0.0f, 0.0f, 1.0f});
}

//--------------------------------------------------------------------
void quad::build_texcoord()
{
  vec2_array& texcoords = *(new vec2_array());
  attrib_array(num_arrays(), array_ptr(&texcoords));
  texcoords.reserve(4);

  texcoords.push_back(m_tc0);
  texcoords.push_back(vec2(m_tc1.x, m_tc0.y));
  texcoords.push_back(m_tc1);
  texcoords.push_back(vec2(m_tc0.x, m_tc1.y));
}

//--------------------------------------------------------------------
void quad::build_tangent()
{
  vec3_array& tangents = *(new vec3_array());
  attrib_array(num_arrays(), array_ptr(&tangents));
  tangents.reserve(4);

  tangents.push_back(vec3{1.0f, 0.0f, 0.0f});
  tangents.push_back(vec3{1.0f, 0.0f, 0.0f});
  tangents.push_back(vec3{1.0f, 0.0f, 0.0f});
  tangents.push_back(vec3{1.0f, 0.0f, 0.0f});
}

//--------------------------------------------------------------------
GLfloat quad::left()
{
  return glm::min(m_v0.x, m_v3.x);
}

//--------------------------------------------------------------------
GLfloat quad::right()
{
  return glm::max(m_v1.x, m_v2.x);
}

//--------------------------------------------------------------------
GLfloat quad::bottom()
{
  return glm::min(m_v0.y, m_v1.y);
}

//--------------------------------------------------------------------
GLfloat quad::top()
{
  return glm::max(m_v2.y, m_v3.y);
}

//--------------------------------------------------------------------
void billboard_quad::rect(const vec2& v0, const vec2& v1)
{
  m_v0 = v0;
  m_v1 = v1;
}

//--------------------------------------------------------------------
void billboard_quad::texcoord(const vec2& tc0, const vec2& tc1)
{
  m_texcoord0 = tc0;
  m_texcoord1 = tc1;
}

//--------------------------------------------------------------------
void billboard_quad::build_vertex()
{
  auto vertices = std::make_shared<vec2_array>();
  attrib_array(0, vertices);
  vertices->push_back(m_v0);
  vertices->push_back(vec2(m_v1.x, m_v0.y));
  vertices->push_back(m_v1);
  vertices->push_back(vec2(m_v0.x, m_v1.y));

  add_primitive_set(new draw_arrays(GL_TRIANGLE_FAN, 0, 4));

}

//--------------------------------------------------------------------
void billboard_quad::build_texcoord()
{
  auto texcoords = std::make_shared<vec2_array>();
  attrib_array(1, texcoords);
  texcoords->push_back(m_texcoord0);
  texcoords->push_back(vec2(m_texcoord1.x, m_texcoord0.y));
  texcoords->push_back(m_texcoord1);
  texcoords->push_back(vec2(m_texcoord0.x, m_texcoord1.y));
}


}
