#ifndef GL4_GM_CIRCLE_H
#define GL4_GM_CIRCLE_H

#include "gm/point.h"

namespace gm
{

class line_2d;
class point_2d;

class circle_2d
{

public:

  circle_2d(const point_2d& c, GLfloat r);

  line_2d tangent_at(const point_2d& p) const;

  std::pair<point_2d, point_2d> intersect(const line_2d& l) const;

  GLfloat get_r() const { return m_r; }
  void set_r(GLfloat v){ m_r = v; }

  const point_2d& get_c() const { return m_c; }
  void set_c(const point_2d& v){ m_c = v; }

private:

  GLfloat m_r;
  point_2d m_c;

};

}

#endif /* GL4_GM_CIRCLE_H */
