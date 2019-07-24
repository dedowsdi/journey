#ifndef GL_GLC_LISSAJOUS_H
#define GL_GLC_LISSAJOUS_H
#include "geometry.h"

namespace zxd
{

class lissajous : public geometry_base
{
public:
  enum LISSA_TYPE
  {
    LT_CIRCLE = 0,
    LT_ROSE
  };
protected:
  GLuint m_xscale = 1;
  GLuint m_yscale = 1;
  GLfloat m_radius = 1;
  GLuint m_slices = 64;
  LISSA_TYPE m_type = LT_CIRCLE;

  GLuint m_rose_n = 1;
  GLuint m_rose_d = 1;
  GLfloat m_rose_offset = 0;

  void build_vertex();

public:

  GLuint xscale() const { return m_xscale; }
  void xscale(GLfloat v){ m_xscale = v; }

  GLuint yscale() const { return m_yscale; }
  void yscale(GLuint v){ m_yscale = v; }

  GLfloat radius() const { return m_radius; }
  void radius(GLfloat v){ m_radius = v; }

  GLuint slices() const { return m_slices; }
  void slices(GLuint v){ m_slices = v; }

  GLfloat period();
  
  glm::vec2 get_at_angle(GLfloat angle) const;
  glm::vec2 tangent_at_angle(GLfloat angle) const;
  glm::vec2 normal_at_angle(GLfloat angle) const;

  LISSA_TYPE type() const { return m_type; }
  void type(LISSA_TYPE v){ m_type = v; }

  GLuint rose_d() const { return m_rose_d; }
  void rose_d(GLuint v){ m_rose_d = v; }

  GLuint rose_n() const { return m_rose_n; }
  void rose_n(GLuint v){ m_rose_n = v; }

  GLfloat rose_offset() const { return m_rose_offset; }
  void rose_offset(GLfloat v){ m_rose_offset = v; }

};
}


#endif /* GL_GLC_LISSAJOUS_H */
