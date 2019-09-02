#include "points.h"

#include "common_program.h"
#include "glm/gtc/type_ptr.hpp"

namespace zxd
{

//--------------------------------------------------------------------
template<typename tvec>
points<tvec>::points(const std::vector<tvec>& points):
  _points(points)
{
}

//--------------------------------------------------------------------
template<typename tvec>
points<tvec>::points(const std::vector<tvec>&& points):
  _points(std::move(points))
{
}

template <typename tvec>
void points<tvec>::set_vertices(const std::vector<tvec>& points)
{
  _points = points;
  rebuild();
}

//--------------------------------------------------------------------
template<typename tvec>
void points<tvec>::set_vertices(std::vector<tvec>&& points)
{
  _points = std::move(points);
  rebuild();
}

//--------------------------------------------------------------------
template<typename tvec>
void points<tvec>::build_vertex() 
{
  rebuild();
}

//--------------------------------------------------------------------
template<typename tvec>
void points<tvec>::rebuild()
{
  auto vertices = make_array<template_array<tvec>>(0);
  vertices->insert(vertices->end(), _points.begin(), _points.end());
  bind_and_update_buffer();

  m_primitive_sets.clear();
  add_primitive_set(new draw_arrays(GL_POINTS, 0, _points.size()));
}

template class points<vec2>;
template class points<vec3>;
template class points<vec4>;

template <typename tvec>
void draw_points(const std::vector<tvec>& points, const mat4& mvp_mat)
{
  static point_program prg;
  if (!prg.is_inited()) prg.init();

  GLuint vao;
  GLuint vbo;
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);

  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(tvec),
    value_ptr(points[0]), GL_STATIC_DRAW);

  glVertexAttribPointer(0, tvec::length(), GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
  glEnableVertexAttribArray(0);

  prg.use();
  prg.udpate_uniforms(mvp_mat);
  glDrawArrays(GL_POINTS, 0, points.size());
}

template void draw_points<vec2>(const std::vector<vec2>&, const mat4&);
template void draw_points<vec3>(const std::vector<vec3>&, const mat4&);
template void draw_points<vec4>(const std::vector<vec4>&, const mat4&);

template <typename tvec>
void draw_points(
  const std::vector<std::vector<tvec>>& points, const mat4& mvp_mat)
{
  for (auto iter = points.begin(); iter != points.end(); ++iter)
  {
    draw_points(*iter, mvp_mat);
  }
}

template void draw_points<vec2>(const std::vector<std::vector<vec2>>&, const mat4&);
template void draw_points<vec3>(const std::vector<std::vector<vec3>>&, const mat4&);
template void draw_points<vec4>(const std::vector<std::vector<vec4>>&, const mat4&);

}
