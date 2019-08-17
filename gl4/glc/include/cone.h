#ifndef GL_GLC_CONE_H
#define GL_GLC_CONE_H

#include "geometry.h"

namespace zxd
{

class cone : public geometry_base
{

public:
  cone() : m_radius(1), m_height(1), m_slice(16), m_stack(16) {}
  cone(GLfloat radius, GLfloat height, GLuint slice, GLuint stack)
      : m_radius(radius), m_height(height), m_slice(slice), m_stack(stack) {}

  GLfloat radius() const { return m_radius; }
  void radius(GLfloat v) { m_radius = v; }

  GLuint slice() const { return m_slice; }
  void slice(GLuint v) { m_slice = v; }

  GLuint stack() const { return m_stack; }
  void stack(GLuint v) { m_stack = v; }

  GLfloat height() const { return m_height; }
  void height(GLfloat v) { m_height = v; }


private:

  void build_vertex() override;
  void build_normal() override;
  void build_texcoord() override;

  GLfloat m_radius;
  GLfloat m_height;
  GLuint m_slice;  // longitiude
  GLuint m_stack;  // latitude
};
}

#endif /* GL_GLC_CONE_H */
