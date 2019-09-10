#ifndef GL_GLC_SUPER_SHAPE_3D_H
#define GL_GLC_SUPER_SHAPE_3D_H

#include "geometry.h"

namespace zxd
{

class super_shape_3d : public common_geometry
{
public:
  enum shape_type
  {
    ST_SPHERE,
    ST_TORUS
  };

  GLfloat n1() const { return m_n1; }
  void n1(GLfloat v){ m_n1 = v; }

  GLfloat n2() const { return m_n2; }
  void n2(GLfloat v){ m_n2 = v; }

  GLfloat n3() const { return m_n3; }
  void n3(GLfloat v){ m_n3 = v; }

  GLfloat m() const { return m_m; }
  void m(GLfloat v){ m_m = v; }

  GLfloat radius() const { return m_radius; }
  void radius(GLfloat v){ m_radius = v; }

  GLfloat a() const { return m_a; }
  void a(GLfloat v){ m_a = v; }

  GLfloat b() const { return m_b; }
  void b(GLfloat v){ m_b = v; }

  GLuint slice() const { return m_slice; }
  void slice(GLuint v){ m_slice = v; }

  GLuint stack() const { return m_stack; }
  void stack(GLuint v){ m_stack = v; }

  GLuint ring() const {return m_stack;}
  void ring(GLuint v){m_stack = v;}

  GLuint side() const {return m_slice;}
  void side(GLuint v){m_slice = v;}

  GLfloat theta_start() const { return m_theta_start; }
  void theta_start(GLfloat v){ m_theta_start = v; }

  GLfloat theta_end() const { return m_theta_end; }
  void theta_end(GLfloat v){ m_theta_end = v; }

  GLfloat phi_start() const { return m_phi_start; }
  void phi_start(GLfloat v){ m_phi_start = v; }

  GLfloat phi_end() const { return m_phi_end; }
  void phi_end(GLfloat v){ m_phi_end = v; }

  shape_type type() const { return m_type; }
  void type(shape_type v){ m_type = v; }

  static std::string shape_type_to_string(shape_type type);
  static shape_type shape_type_from_string(const std::string& s);

private:
  GLfloat supershape(GLfloat theta);

  vertex_build build_vertices() override;
  vertex_build build_sphere_vertex();
  vertex_build build_torus_vertex();

  shape_type m_type = ST_SPHERE;
  GLfloat m_n1 = 1;
  GLfloat m_n2 = 1;
  GLfloat m_n3 = 1;
  GLfloat m_m = 0;
  GLfloat m_radius = 1;
  GLfloat m_a = 1;
  GLfloat m_b = 1;

  GLuint m_slice = 32; // theta subdivisions or side
  GLuint m_stack = 32; // phi subdivisions or ring
  GLfloat m_theta_start = -zxd::fpi; // second loop angle, sphere will be build stack by stack
  GLfloat m_theta_end = zxd::fpi;
  GLfloat m_phi_start = -zxd::fpi2; // first loop angle, torus will be build tube by tube
  GLfloat m_phi_end = zxd::fpi2;

};

}

#endif /* GL_GLC_SUPER_SHAPE_3D_H */
