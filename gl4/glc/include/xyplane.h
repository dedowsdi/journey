#ifndef GL_GLC_XYPLANE_H
#define GL_GLC_XYPLANE_H

#include "geometry.h"
#include "glm.h"

namespace zxd
{

class xyplane : public common_geometry
{

public:

  xyplane(GLfloat width = 1.0f, GLfloat height = 1.0f, GLuint slice = 1);

  xyplane(GLfloat x0, GLfloat y0, GLfloat x1, GLfloat y1, GLuint slice = 1);

  GLuint slice() const { return m_slice; }
  void slice(GLuint v) { m_slice = v; }

  GLfloat width() const { return m_width; }
  void width(GLfloat v) { m_width = v; }

  GLfloat height() const { return m_height; }
  void height(GLfloat v) { m_height = v; }

  GLfloat left() const { return m_left; }
  void left(GLfloat v){ m_left = v; }

  GLfloat bottom() const { return m_bottom; }
  void bottom(GLfloat v){ m_bottom = v; }

private:

  vertex_build build_vertices() override;
  array_uptr build_normals(const array& vertices) override;
  array_uptr build_texcoords(const array& vertices) override;

  GLuint m_slice;
  GLfloat m_width;
  GLfloat m_height;
  GLfloat m_left;
  GLfloat m_bottom;

};
}

#endif /* GL_GLC_XYPLANE_H */
