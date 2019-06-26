#ifndef GL_GLC_SPHERE_H
#define GL_GLC_SPHERE_H

#include "geometry.h"
#include "glm.h"

namespace zxd
{

/*
 * build sphere with two triangle fans and a bunch of triangle strips
 */
class sphere : public geometry_base
{
protected:
  GLfloat m_radius;
  GLuint m_slice;  // longitiude
  GLuint m_stack;  // latitude

public:
  sphere() : m_radius(1), m_slice(16), m_stack(16) {}
  sphere(GLfloat radius, GLuint slice, GLuint stack)
      : m_radius(radius), m_slice(slice), m_stack(stack) {}

  void build_vertex();
  // as i'm using strip and fan, normal should be vertex normal.
  void build_normal();
  void build_texcoord();

  GLfloat radius() const { return m_radius; }
  void radius(GLfloat v) { m_radius = v; }

  GLuint slice() const { return m_slice; }
  void slice(GLuint v) { m_slice = v; }

  GLuint stack() const { return m_stack; }
  void stack(GLuint v) { m_stack = v; }

  static vec3_vector get_sphere_points(GLfloat radius, GLuint slices, GLuint stacks);
};
}

#endif /* GL_GLC_SPHERE_H */
