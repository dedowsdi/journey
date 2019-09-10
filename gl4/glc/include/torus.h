#ifndef GL_GLC_TORUS_H
#define GL_GLC_TORUS_H

#include "geometry.h"
#include "glm.h"

namespace zxd
{

// http://paulbourke.net/geometry/toroidal/
class torus : public common_geometry
{

public:
  torus();
  torus(GLfloat inner_radius, GLfloat outer_radius, GLuint sides, GLuint rings);

  GLfloat inner_radius() const { return m_inner_radius; }
  void inner_radius(GLfloat v) { m_inner_radius = v; }

  GLfloat outer_radius() const { return m_outer_radius; }
  void outer_radius(GLfloat v) { m_outer_radius = v; }

  GLuint sides() const { return m_sides; }
  void sides(GLuint v) { m_sides = v; }

  GLuint rings() const { return m_rings; }
  void rings(GLuint v) { m_rings = v; }

private:

  vertex_build build_vertices() override;

  GLfloat m_inner_radius;
  GLfloat m_outer_radius;
  GLuint m_sides;  // longitiude
  GLuint m_rings;  // latitude

};
}

#endif /* GL_GLC_TORUS_H */
