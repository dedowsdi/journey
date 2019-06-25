#ifndef GL_GLC_POINTS_H
#define GL_GLC_POINTS_H
#include "geometry.h"
#include "common_program.h"

namespace zxd
{

template <typename tvec>
class points : public geometry_base
{
public:

  GLuint build_mesh(const std::vector<tvec>& points)
  {
    template_array<tvec>& vertices = *(new template_array<tvec>);
    attrib_array(num_arrays(), array_ptr(&vertices));
    vertices.insert(vertices.end(), points.begin(), points.end());
    bind_and_update_buffer();

    m_primitive_sets.clear();
    add_primitive_set(new draw_arrays(GL_POINTS, 0, points.size()));
  }

};

class origin2 : public geometry_base
{
protected:
  void build_vertex();

public:
};

// draw points, it cretes buffer every time you called it, don't use it if
// performance is an issue.
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

  glVertexAttribPointer(0, tvec::components, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
  glEnableVertexAttribArray(0);

  prg.use();
  prg.udpate_uniforms(mvp_mat);
  glDrawArrays(GL_POINTS, 0, points.size());

  // QUES : should i release vao and vbo?
}

template <typename tvec>
void draw_points(
  const std::vector<std::vector<tvec>>& points, const mat4& mvp_mat)
{
  for (auto iter = points.begin(); iter != points.end(); ++iter)
  {
    draw_points(*iter, mvp_mat);
  }
}

}

#endif /* GL_GLC_POINTS_H */
