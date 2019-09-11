#ifndef GL_GLC_CONE_H
#define GL_GLC_CONE_H

#include <geometry.h>

namespace zxd
{

class cone : public common_geometry
{

public:
  cone() = default;
  cone(GLfloat radius, GLfloat height, GLuint slice, GLuint stack);

  GLfloat radius() const { return m_radius; }
  void radius(GLfloat v) { m_radius = v; }

  GLuint slice() const { return m_slice; }
  void slice(GLuint v) { m_slice = v; }

  GLuint stack() const { return m_stack; }
  void stack(GLuint v) { m_stack = v; }

  GLfloat height() const { return m_height; }
  void height(GLfloat v) { m_height = v; }

private:

  vertex_build build_vertices() override;
  array_uptr build_normals(const array& vertices) override;
  array_uptr build_texcoords(const array& vertices) override;

  GLfloat m_radius{1};
  GLfloat m_height{1};
  GLuint m_slice{16};  // longitiude
  GLuint m_stack{16};  // latitude
};
}

#endif /* GL_GLC_CONE_H */
