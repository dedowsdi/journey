#ifndef GL4_GLC_CAPSULE3D_H
#define GL4_GLC_CAPSULE3D_H

#include "geometry.h"

namespace zxd
{

// slice, stack must be even number
class capsule3d : public geometry_base
{
public:

  GLfloat radius() const { return m_radius; }
  void radius(GLfloat v){ m_radius = v; }

  GLuint sphere_slice() const { return m_sphere_slice; }
  void sphere_slice(GLuint v){ m_sphere_slice = v; }

  GLuint sphere_stack() const { return m_sphere_stack; }
  void sphere_stack(GLuint v){ m_sphere_stack = v; }

  GLuint cylinder_stack() const { return m_cylinder_stack; }
  void cylinder_stack(GLuint v){ m_cylinder_stack = v; }

  GLfloat height() const { return m_height; }
  void height(GLfloat v){ m_height = v; }

  void build_vertex() override;
  void build_normal() override;
  void build_texcoord() override;

private:
  GLfloat m_radius = 0.25f;
  GLfloat m_height= 1.0f; // total height
  GLuint m_sphere_slice = 8;  // longitiude
  GLuint m_sphere_stack = 8;  // latitude
  GLuint m_cylinder_stack = 1;

};

}

#endif /* GL4_GLC_CAPSULE3D_H */
