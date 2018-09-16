#ifndef POINTS_H
#define POINTS_H
#include "geometry.h"
#include "common_program.h"
#include "common.h"

namespace zxd {

class Points3 : public geometry_base {
protected:
  vec3_vector m_points;

public:
  virtual void draw(GLuint primcount);
};

class origin2 : public geometry_base{
protected:
  GLint build_vertex();

public:
  virtual void draw(GLuint primcount);
};

// draw points, it cretes buffer every time you called it, don't use it if
// performance is an issue.
template <typename tvec>
void draw_points(const std::vector<tvec>& points, const mat4& mvp_mat) {
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

  glVertexAttribPointer(
    prg.al_vertex, tvec::components, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
  glEnableVertexAttribArray(prg.al_vertex);

  prg.use();
  prg.udpate_uniforms(mvp_mat);
  glDrawArrays(GL_POINTS, 0, points.size());

  // QUES : should i release vao and vbo?
}

template <typename tvec>
void draw_points(
  const std::vector<std::vector<tvec>>& points, const mat4& mvp_mat) {
  for (auto iter = points.begin(); iter != points.end(); ++iter) {
    draw_points(*iter, mvp_mat);
  }
}
}

#endif /* POINTS_H */
