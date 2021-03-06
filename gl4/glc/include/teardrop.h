#ifndef GL4_GLC_TEARDROP_H
#define GL4_GLC_TEARDROP_H

// http://paulbourke.net/geometry/teardrop/   θ, φ swapped

#include <geometry.h>
#include <glm.h>

namespace zxd
{

// z in [-1, 1]
// x,y in (-xy_scale*2, xy_scale*2)
class teardrop : public common_geometry
{

public:

  teardrop(GLfloat radius = 1, GLuint slice = 16, GLuint stack = 16,
    GLfloat xy_scale = 0.5f);

  GLfloat xy_scale() const { return m_xy_scale; }
  void xy_scale(GLfloat v){ m_xy_scale = v; }

  GLfloat radius() const { return m_radius; }
  void radius(GLfloat v) { m_radius = v; }

  GLuint slice() const { return m_slice; }
  void slice(GLuint v) { m_slice = v; }

  GLuint stack() const { return m_stack; }
  void stack(GLuint v) { m_stack = v; }

  static vec3_vector get_points(GLfloat radius, GLuint slices, GLuint stacks, GLfloat xy_scale = 0.5);

private:

  vertex_build build_vertices() override;
  array_ptr build_texcoords(const array& vertices) override;

  GLfloat m_radius;
  GLfloat m_xy_scale;
  GLuint m_slice;  // longitiude
  GLuint m_stack;  // latitude

};
}

#endif /* GL4_GLC_TEARDROP_H */
