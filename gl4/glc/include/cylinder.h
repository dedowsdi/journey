#ifndef GL_GLC_CYLINDER_H
#define GL_GLC_CYLINDER_H

#include "geometry.h"
#include "glm.h"

namespace zxd
{

class cylinder : public geometry_base
{
public:

  cylinder();

  cylinder(
    GLfloat base, GLfloat top, GLfloat height, GLuint slice, GLuint stack);

  GLuint slice() const { return m_slice; }
  void slice(GLuint v) { m_slice = v; }

  GLuint stack() const { return m_stack; }
  void stack(GLuint v) { m_stack = v; }

  GLfloat height() const { return m_height; }
  void height(GLfloat v) { m_height = v; }

  GLfloat base() const { return m_base; }
  void base(GLfloat v) { m_base = v; }

  GLfloat top() const { return m_top; }
  void top(GLfloat v) { m_top = v; }

private:

  void build_vertex() override;
  void build_normal() override;
  void build_texcoord() override;
  void on_draw() override;

  GLfloat m_base;
  GLfloat m_top;
  GLfloat m_height;
  GLuint m_slice;
  GLuint m_stack;

};
}

#endif /* GL_GLC_CYLINDER_H */
