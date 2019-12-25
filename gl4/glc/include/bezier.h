#ifndef GL_GLC_BEZIER_H
#define GL_GLC_BEZIER_H

#include <geometry.h>

namespace zxd
{

/*
 * generally bezier class, degree >=1
 */
class bezier
{
public:
  bezier() : m_partitions(20), m_begin(0), m_end(1) {}

  unsigned int degree() { return n(); }
  unsigned int n() { return m_ctrl_points.size() - 1; }

  // get point at t
  glm::vec3 get(GLfloat t);

  // get point at ith iteration, jth index
  glm::vec3 get(GLuint i, GLuint j, GLfloat t);

  // degree evevate or drop
  void elevate(bool positive = true);

  // derivative of a bezier curve is still a bezier curve
  bezier derivative(GLuint level = 1);

  // get tanget at t
  glm::vec3 tangent(GLfloat t);

  // subdivide according to current point
  void subdivide(GLfloat t, bezier& lc, bezier& rc);

  unsigned int partitions() const { return m_partitions; }
  void partitions(unsigned int v) { m_partitions = v; }

  GLfloat begin() const { return m_begin; }
  void begin(GLfloat v) { m_begin = v; }

  GLfloat end() const { return m_end; }
  void end(GLfloat v) { m_end = v; }

  const vec3_vector& ctrl_points() const { return m_ctrl_points; }
  void ctrl_points(const vec3_vector& v) { m_ctrl_points = v; }

  static vec3_vector2 iterate_all(vec3_vector& ctrl_points, float t);
  static vec3_vector2 iterate_all(
    vec3_vector::const_iterator beg, vec3_vector::const_iterator end, float t);

  // De Casteljau's algorithm
  static vec3_vector iterate(vec3_vector& ctrl_points, float t);
  static vec3_vector iterate(
    vec3_vector::const_iterator beg, vec3_vector::const_iterator end, float t);

  static glm::vec3 get(vec3_vector::const_iterator beg,
    vec3_vector::const_iterator end, GLfloat t);

  static glm::vec3 tangent(vec3_vector::const_iterator beg,
    vec3_vector::const_iterator end, GLfloat t);

private:
  // special function used to derivative
  glm::vec3 d(GLuint i, GLuint k);

  GLuint m_partitions;  // number of partitions
  GLfloat m_begin;
  GLfloat m_end;

  vec3_vector m_ctrl_points;
};

class bezier_geom : public common_geometry
{
public:
  bezier_geom(const bezier& shape);

  const bezier& get_shape() const { return _shape; }
  void set_shape(const bezier& v){ _shape = v; }

private:
  vertex_build build_vertices() override;
  array_ptr build_texcoords(const array& vertices) override;

  bezier _shape;
};

}

#endif /* GL_GLC_BEZIER_H */
