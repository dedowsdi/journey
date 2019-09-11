#ifndef GL_GLC_CYLINDER_H
#define GL_GLC_CYLINDER_H

#include <geometry.h>
#include <glm.h>

namespace zxd
{

class cylinder : public common_geometry
{
public:
  cylinder(GLfloat base = 1, GLfloat top = 1, GLfloat height = 1,
    GLuint slice = 16, GLuint stack = 16);

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

  vertex_build build_vertices() override;
  array_uptr build_normals(const array& vertices) override;
  array_uptr build_texcoords(const array& vertices) override;

  GLfloat m_base;
  GLfloat m_top;
  GLfloat m_height;
  GLuint m_slice;
  GLuint m_stack;

};
}

#endif /* GL_GLC_CYLINDER_H */
