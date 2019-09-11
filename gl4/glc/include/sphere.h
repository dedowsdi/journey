#ifndef GL_GLC_SPHERE_H
#define GL_GLC_SPHERE_H

#include <geometry.h>

namespace zxd
{

class sphere : public common_geometry
{

public:
  sphere() = default;
  sphere(GLfloat radius, GLuint slice, GLuint stack);

  GLfloat radius() const { return m_radius; }
  void radius(GLfloat v) { m_radius = v; }

  GLuint slice() const { return m_slice; }
  void slice(GLuint v) { m_slice = v; }

  GLuint stack() const { return m_stack; }
  void stack(GLuint v) { m_stack = v; }

  static vec3_vector create_points(GLfloat radius, GLuint slices, GLuint stacks);

private:

  vertex_build build_vertices() override;
  array_uptr build_normals(const array& vertices) override;
  array_uptr build_texcoords(const array& vertices) override;

  GLfloat m_radius{1};
  GLuint m_slice{16};  // longitiude
  GLuint m_stack{16};  // latitude

};
}

#endif /* GL_GLC_SPHERE_H */
