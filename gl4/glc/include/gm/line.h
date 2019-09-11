#ifndef GL4_GM_LINE_H
#define GL4_GM_LINE_H

#include "gm/point.h"

namespace gm
{

class line_2d
{

public:

  line_2d(const point_2d& p0, const point_2d& p1);

  // P = U + tv, the explicit form
  line_2d(const vec2& v, const point_2d& u);

  // n·P + c = 0, the implicit form
  line_2d(const vec2& n, GLfloat c);

  // nearest point on this line to origin
  point_2d U() const;

  // direction vector
  vec2 v() const;

  // perendicular vector (in ccw order from v)
  vec2 n() const;

  bool normalized() const;

  void normalize() const;

  line_2d perp_at(const point_2d& p) const;

private:

  vec2 m_v;
  point_2d m_u;
};

}

#endif /* GL4_GM_LINE_H */
