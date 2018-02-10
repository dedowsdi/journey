#ifndef NURB_H
#define NURB_H

#include <algorithm>
#include "geometry.h"

namespace zxd {
using namespace glm;
// clamped b-spline
class nurb : public geometry4313 {
protected:
  vec4_vector m_ctrl_points;
  float_vector m_knots;
  GLuint m_degree;
  unsigned int m_partitions;  // number of segments
  GLfloat m_begin;
  GLfloat m_end;

public:
  nurb() : m_partitions(20), m_begin(0.0f), m_end(1.0f) {}
  ~nurb() {}

  GLuint m() { return m_knots.size() - 1; }
  GLuint n() { return m_ctrl_points.size() - 1; }
  GLuint p() { return m_degree; }
  bool valid() { return m() == n() + p() + 1; }

  virtual void build_vertex();
  virtual void build_texcoord();
  virtual void draw(GLuint primcount = 1);

  // get point by De Boor's algorithm
  vec4 get(GLfloat u);

  // compute pointer directly by b-spline formula
  vec4 get_by_coefs(GLfloat u);

  void insert_knot(GLfloat u, GLuint times = 1);

  // a knot span is [u_k, u_k+1), there are at most m-2p spans in clamped b
  // spline
  GLuint knot_span(GLfloat u);

  GLfloat min_knot() { return m_knots.front(); }
  GLfloat max_knot() { return m_knots.back(); }

  GLuint knot_multiplicity(GLuint i);

  void subdivide(GLfloat u, nurb& lc, nurb& rc);

  std::vector<vec4_vector> iterate(
    GLuint beg, GLuint end, GLfloat u, GLuint times);

  GLuint degree() const { return m_degree; }
  void degree(GLuint v) { m_degree = v; }
  void update_degree() { m_degree = m() - n() - 1; }

  unsigned int partitions() const { return m_partitions; }
  void partitions(unsigned int v) { m_partitions = v; }

  const vec4_vector& ctrl_points() const { return m_ctrl_points; }
  void ctrl_points(const vec4_vector& v) { m_ctrl_points = v; }

  GLfloat begin() const { return m_begin; }
  void begin(GLfloat v){ m_begin = v; }

  GLfloat end() const { return m_end; }
  void end(GLfloat v){ m_end = v; }

  const float_vector& knots() const { return m_knots; }
  void knots(const float_vector& v) { m_knots = v; }
  GLfloat getKnot(GLuint index) const { return m_knots.at(index); }
  // created clamped uniform knots
  void uniform_knots();

protected:
  float_vector coefficents(GLfloat u);

  GLfloat getKnotRatio(GLfloat t, GLuint i, GLuint r = 1);
};
}

#endif /* NURB_H */
