#ifndef GL_GLC_NURBSURFACE_H
#define GL_GLC_NURBSURFACE_H
#include "geometry.h"

namespace zxd
{
class nurb_surface : public geometry_base
  {
protected:
  GLfloat m_ubegin;
  GLfloat m_uend;
  GLfloat m_vbegin;
  GLfloat m_vend;
  GLuint m_upartition;
  GLuint m_vpartition;
  GLuint m_udegree;
  GLuint m_vdegree;

  vec4_vector2 m_ctrl_points;  // col major
  float_vector m_uknots;
  float_vector m_vknots;

public:
  nurb_surface()
      : m_ubegin(0),
        m_uend(1),
        m_vbegin(0),
        m_vend(1),
        m_upartition(20),
        m_vpartition(20),
        m_udegree(3),
        m_vdegree(3){};

  virtual void build_vertex();

  virtual void build_normal();

  virtual void build_texcoord();

  vec4 get(GLfloat u, GLfloat v);

  // get interim horizontal control points as specific u
  vec4_vector u_interim(GLfloat u);

  // get interim vertical control points as specific u
  vec4_vector v_interim(GLfloat v);

  // get all horizontal control points
  vec4_vector2 u_interim2();

  // get all vertical control points
  vec4_vector2 v_interim2();

  const vec4_vector& col(GLuint i);
  vec4_vector row(GLuint i);

  void uniform_knots();
  void uniform_uknots();
  void uniform_vknots();

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

  const vec4_vector2& ctrl_points() const { return m_ctrl_points; }
  void ctrl_points(const vec4_vector2& v) { m_ctrl_points = v; }

  const float_vector& uknots() const { return m_uknots; }
  void uknots(const float_vector& v) { m_uknots = v; }

  const float_vector& vknots() const { return m_vknots; }
  void vknots(const float_vector& v) { m_vknots = v; }

  GLuint udegree() const { return m_udegree; }
  void udegree(GLuint v) { m_udegree = v; }

  GLuint vdegree() const { return m_vdegree; }
  void vdegree(GLuint v) { m_vdegree = v; }

  GLuint up() { return udegree(); }
  GLuint vp() { return vdegree(); }
  GLuint uorder()
  {
    return m_ctrl_points.empty() ? 0 : m_ctrl_points[0].size();
  }
  GLuint vorder() { return m_ctrl_points.size(); }
  GLuint un() { return uorder() - 1; }
  GLuint vn() { return vorder() - 1; }
};
}

#endif /* GL_GLC_NURBSURFACE_H */
