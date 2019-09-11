#ifndef GL_GLC_BEZIERSURFACE_H
#define GL_GLC_BEZIERSURFACE_H
#include <geometry.h>

namespace zxd
{
class bezier_surface : public common_geometry
{

public:
  bezier_surface(GLfloat ubegin = 0, GLfloat uend = 1, GLfloat vbegin = 0,
    GLfloat vend = 1, GLuint upartition = 20, GLuint vpartition = 20);

  virtual vertex_build build_vertices();

  virtual array_uptr build_normals(const array& vertices);

  virtual array_uptr build_texcoords(const array& vertices);

  glm::vec3 get(GLfloat u, GLfloat v);

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

private:
  GLfloat m_ubegin;
  GLfloat m_uend;
  GLfloat m_vbegin;
  GLfloat m_vend;
  GLuint m_upartition;
  GLuint m_vpartition;

  vec3_vector2 m_ctrl_points;  // col major

};
}

#endif /* GL_GLC_BEZIERSURFACE_H */
