#ifndef SUPER_SHAPE_3D_H
#define SUPER_SHAPE_3D_H

#include "geometry.h"
#include "glm.h"

namespace zxd
{

class super_shape_3d : public geometry_base
{
protected:
  GLfloat m_n1 = 1;
  GLfloat m_n2 = 1;
  GLfloat m_n3 = 1;
  GLfloat m_m = 0;
  GLfloat m_radius = 1;
  GLfloat m_a = 1;
  GLfloat m_b = 1;

  GLuint m_slice = 32; // longitude
  GLuint m_stack = 32; // latitude
  GLfloat m_longitude_start = -zxd::fpi;
  GLfloat m_longitude_end = zxd::fpi;
  GLfloat m_latitude_start = -zxd::fpi2;
  GLfloat m_latitude_end = zxd::fpi2;

public:
  virtual void build_vertex();

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

  GLfloat longitude_start() const { return m_longitude_start; }
  void longitude_start(GLfloat v){ m_longitude_start = v; }

  GLfloat longitude_end() const { return m_longitude_end; }
  void longitude_end(GLfloat v){ m_longitude_end = v; }

  GLfloat latitude_start() const { return m_latitude_start; }
  void latitude_start(GLfloat v){ m_latitude_start = v; }

  GLfloat latitude_end() const { return m_latitude_end; }
  void latitude_end(GLfloat v){ m_latitude_end = v; }

protected:
  GLfloat supershape(GLfloat theta);

};

}


#endif /* SUPER_SHAPE_3D_H */
