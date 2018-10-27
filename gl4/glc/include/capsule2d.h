#ifndef CAPSULE2_H
#define CAPSULE2_H
#include "geometry.h"

namespace zxd
{

class capsule2d : public geometry_base
{
protected:
  GLfloat m_radius = 1;
  GLfloat m_width = 4; // total width
  GLuint m_slice = 32;

public:

  GLfloat radius() const { return m_radius; }
  void radius(GLfloat v){ m_radius = v; }

  GLfloat width() const { return m_width; }
  void width(GLfloat v){ m_width = v; }

  GLuint slice() const { return m_slice; }
  void slice(GLuint v){ m_slice = v; }

  void build_vertex();

};

}


#endif /* CAPSULE2_H */
