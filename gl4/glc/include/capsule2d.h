#ifndef GL_GLC_CAPSULE2_H
#define GL_GLC_CAPSULE2_H
#include "geometry.h"

namespace zxd
{

class capsule2d : public geometry_base
{
public:
  enum CAPSULE_TYPE
  {
    CT_CIRCLE,
    CT_POINT
  };
protected:
  GLfloat m_radius = 0.125;
  GLfloat m_width = 1; // total width
  GLuint m_slice = 32;
  CAPSULE_TYPE m_type = CT_CIRCLE;

public:

  GLfloat radius() const { return m_radius; }
  void radius(GLfloat v){ m_radius = v; }

  GLfloat width() const { return m_width; }
  void width(GLfloat v){ m_width = v; }

  GLuint slice() const { return m_slice; }
  void slice(GLuint v){ m_slice = v; }

  void build_vertex();

  CAPSULE_TYPE type() const { return m_type; }
  void type(CAPSULE_TYPE v){ m_type = v; }
};

}


#endif /* GL_GLC_CAPSULE2_H */
