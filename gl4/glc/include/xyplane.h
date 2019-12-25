#ifndef GL_GLC_XYPLANE_H
#define GL_GLC_XYPLANE_H

#include <geometry.h>
#include <glm.h>

namespace zxd
{

class xyplane : public common_geometry
{

public:

  xyplane(GLfloat width = 1.0f, GLfloat height = 1.0f, GLuint slice = 1);

  xyplane(GLfloat x0, GLfloat y0, GLfloat x1, GLfloat y1, GLuint slice = 1);

  GLuint slice() const { return m_slice; }
  void slice(GLuint v) { m_slice = v; }

  void set_texcoord(const vec2& texcoord0, const vec2& texcoord1);

private:

  vertex_build build_vertices() override;
  array_ptr build_normals(const array& vertices) override;
  array_ptr build_texcoords(const array& vertices) override;

  GLuint m_slice{1};
  vec2 m_point0{-1};
  vec2 m_point1{1};
  vec2 m_texcoord0{0};
  vec2 m_texcoord1{1};

};
}

#endif /* GL_GLC_XYPLANE_H */
