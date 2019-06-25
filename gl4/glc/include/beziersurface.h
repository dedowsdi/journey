#ifndef GL_GLC_BEZIERSURFACE_H
#define GL_GLC_BEZIERSURFACE_H
#include "geometry.h"

namespace zxd
{
class bezier_surface : public geometry_base
  {
protected:
  GLfloat m_ubegin;
  GLfloat m_uend;
  GLfloat m_vbegin;
  GLfloat m_vend;
  GLuint m_upartition;
  GLuint m_vpartition;

  vec3_vector2 m_ctrl_points;  // col major

public:
  bezier_surface()
      : m_ubegin(0),
        m_uend(1),
        m_vbegin(0),
        m_vend(1),
        m_upartition(20),
        m_vpartition(20){};

  virtual void build_vertex();

  virtual void build_normal();

  virtual void build_texcoord();

  vec3 get(GLfloat u, GLfloat v);

  // get interim horizontal control points as specific u
  vec3_vector u_interim(GLfloat u);

  // get interim vertical control points as specific u
  vec3_vector v_interim(GLfloat v);

  // get all horizontal control points
  vec3_vector2 u_interim2();

  // get all vertical control points
  vec3_vector2 v_interim2();

  const vec3_vector& col(GLuint i);
  vec3_vector row(GLuint i);

  GLuint uorder();
  GLuint vorder();

  GLfloat ubegin() const { return m_ubegin; }
  void ubegin(GLfloat v) { m_ubegin = v; }

  GLfloat uend() const { return m_uend; }
  void uend(GLfloat v) { m_uend = v; }

  GLfloat vbegin() const { return m_vbegin; }
  void vbegin(GLfloat v) { m_vbegin = v; }

  GLfloat vend() const { return m_vend; }
  void vend(GLfloat v) { m_vend = v; }

  GLuint upartition() const { return m_upartition; }
  void upartition(GLuint v) { m_upartition = v; }

  GLuint vpartition() const { return m_vpartition; }
  void vpartition(GLuint v) { m_vpartition = v; }

  const vec3_vector2& ctrl_points() const { return m_ctrl_points; }
  void ctrl_points(const vec3_vector2& v) { m_ctrl_points = v; }
};
}

#endif /* GL_GLC_BEZIERSURFACE_H */
