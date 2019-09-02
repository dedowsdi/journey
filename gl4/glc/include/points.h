#ifndef GL_GLC_POINTS_H
#define GL_GLC_POINTS_H
#include "geometry.h"

namespace zxd
{

template <typename tvec>
class points : public geometry_base
{
public:
  points(const std::vector<tvec>& points =  {});
  points(const std::vector<tvec>&& points);

  void set_vertices(const std::vector<tvec>& points);
  void set_vertices(std::vector<tvec>&& points);

  void build_vertex() override;

private:

  void rebuild();

  std::vector<tvec> _points;
};

// draw points, it cretes vertex and buffer every time you called it, don't use
// it if performance is an issue.
template <typename tvec>
void draw_points(const std::vector<tvec>& points, const mat4& mvp_mat);

template <typename tvec>
void draw_points(
  const std::vector<std::vector<tvec>>& points, const mat4& mvp_mat);
}

#endif /* GL_GLC_POINTS_H */
