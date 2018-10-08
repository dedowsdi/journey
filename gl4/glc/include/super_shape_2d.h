#ifndef SUPER_SHAPE_2D_H
#define SUPER_SHAPE_2D_H
#include "geometry.h"

namespace zxd
{

// http://paulbourke.net/geometry/supershape/
class super_shape_2d : public geometry_base
{
protected:
  GLfloat m_n1 = 1;
  GLfloat m_n2 = 1;
  GLfloat m_n3 = 1;
  GLfloat m_a = 1;
  GLfloat m_b = 1;
  GLfloat m_m = 1;
  GLfloat m_radius = 1;
  GLuint m_num_slice = 64;
  
public:

  GLfloat n1() const { return m_n1; }
  void n1(GLfloat v){ m_n1 = v; }

  GLfloat n2() const { return m_n2; }
  void n2(GLfloat v){ m_n2 = v; }

  GLfloat n3() const { return m_n3; }
  void n3(GLfloat v){ m_n3 = v; }

  GLfloat a() const { return m_a; }
  void a(GLfloat v){ m_a = v; }

  GLfloat b() const { return m_b; }
  void b(GLfloat v){ m_b = v; }

  GLfloat m() const { return m_m; }
  void m(GLfloat v){ m_m = v; }

  GLfloat radius() const { return m_radius; }
  void radius(GLfloat v){ m_radius = v; }

  GLuint num_slice() const { return m_num_slice; }
  void num_slice(GLuint v){ m_num_slice = v; }

  virtual void build_vertex();

};

}



#endif /* SUPER_SHAPE_2D_H */
