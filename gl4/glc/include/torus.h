#ifndef TORUS_H
#define TORUS_H

#include "geometry.h"
#include "glm.h"

namespace zxd {

/*
 * build sphere with two triangle fans and a bunch of triangle strips
 */
class torus : public geometry_base {
protected:
  GLfloat m_inner_radius;
  GLfloat m_outer_radius;
  GLuint m_sides;  // longitiude
  GLuint m_rings;  // latitude

public:
  torus() : m_inner_radius(0.25), m_outer_radius(1), m_sides(16), m_rings(16) {}
  torus(GLfloat inner_radius, GLfloat outer_radius, GLuint sides, GLuint rings)
      : m_inner_radius(inner_radius),
        m_outer_radius(outer_radius),
        m_sides(sides),
        m_rings(rings) {}

  void build_vertex();
  // as i'm using strip and fan, normal should be vertex normal.
  void build_normal();
  void build_texcoord();

  GLfloat inner_radius() const { return m_inner_radius; }
  void inner_radius(GLfloat v) { m_inner_radius = v; }

  GLfloat outer_radius() const { return m_outer_radius; }
  void outer_radius(GLfloat v) { m_outer_radius = v; }

  GLuint sides() const { return m_sides; }
  void sides(GLuint v) { m_sides = v; }

  GLuint rings() const { return m_rings; }
  void rings(GLuint v) { m_rings = v; }

protected:
  vec3 sphere_vertex();

};
}

#endif /* TORUS_H */
