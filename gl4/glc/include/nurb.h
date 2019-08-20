#ifndef GL_GLC_NURB_H
#define GL_GLC_NURB_H

#include <algorithm>
#include "geometry.h"

namespace zxd
{
using namespace glm;

// clamped b-spline
class nurb : public geometry_base
{
public:
  nurb(v4v_ci cbeg, v4v_ci cend, fv_ci kbeg, fv_ci kend, GLuint degree)
      : m_ctrl_points(cbeg, cend), m_knots(kbeg, kend), m_degree(degree) {}
  nurb() : m_partitions(20), m_begin(0.0f), m_end(1.0f) {}
  ~nurb() {}

  GLuint m() { return m_knots.size() - 1; }
  GLuint n() { return m_ctrl_points.size() - 1; }
  GLuint p() { return m_degree; }
  bool valid() { return m() == n() + p() + 1; }

  // get point by De Boor's algorithm
  glm::vec4 get(GLfloat u);
  glm::vec3 tangent(GLfloat u);
  nurb derivative();

  // compute pointer directly by b-spline formula
  glm::vec4 get_by_coefs(GLfloat u);

  void insert_knot(GLfloat u, GLuint times = 1);

  // a knot span is [u_k, u_k+1), there are at most m-2p spans in clamped b
  // spline
  GLuint knot_span(GLfloat u);

  GLfloat min_knot() { return m_knots.front(); }
  GLfloat max_knot() { return m_knots.back(); }

  GLfloat weight(GLint i) { return m_ctrl_points[i].w; }
  void weight(GLint i, GLfloat w)
  {
    // what if current w is 0?
    m_ctrl_points[i] *= w / m_ctrl_points[i].w;
  }

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
  void begin(GLfloat v) { m_begin = v; }

  GLfloat end() const { return m_end; }
  void end(GLfloat v) { m_end = v; }

  const float_vector& knots() const { return m_knots; }
  void knots(const float_vector& v) { m_knots = v; }
  void knots(fv_ci kbeg, fv_ci kend) { m_knots.assign(kbeg, kend); }
  GLfloat knot(GLuint index) const { return m_knots.at(index); }
  // created clamped uniform knots
  void uniform_knots();
  GLfloat coefficient(GLint i, GLint p, GLfloat u);

  static glm::vec4 get(
    v4v_ci cbeg, v4v_ci cend, fv_ci kbeg, fv_ci kend, GLuint degree, GLfloat u);
  static glm::vec3 tangent(
    v4v_ci cbeg, v4v_ci cend, fv_ci kbeg, fv_ci kend, GLuint degree, GLfloat u);

  static float coefficient(
    fv_ci kbeg, fv_ci kend, GLuint i, GLuint p, GLfloat u);
  static float_vector coefficients(fv_ci kbeg, fv_ci kend, GLuint p, GLfloat u);
  static GLuint knot_span(fv_ci kbeg, fv_ci kend, GLuint p, GLfloat u);

private:

  float_vector coefficients(GLfloat u);
  void build_vertex() override;
  void build_texcoord() override;
  GLfloat knot_ratio(GLfloat t, GLuint i, GLuint r = 1);

  vec4_vector m_ctrl_points;
  float_vector m_knots;
  GLuint m_degree;
  unsigned int m_partitions;  // number of segments
  GLfloat m_begin;
  GLfloat m_end;

};
}

#endif /* GL_GLC_NURB_H */
