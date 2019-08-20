#ifndef GL_GLC_CIRCLE_H
#define GL_GLC_CIRCLE_H

#include "geometry.h"
namespace zxd
{

class circle : public geometry_base
{

public:

  enum CIRCLE_TYPE
  {
    FILL,
    LINE,
    PIE_LINE,
  };

  GLfloat radius() const { return m_radius; }
  void radius(GLfloat v){ m_radius = v; }

  CIRCLE_TYPE type() const { return m_type; }
  void type(CIRCLE_TYPE v){ m_type = v; }

  GLuint slice() const { return m_slice; }
  void slice(GLuint v){ m_slice = v; }

  GLfloat start() const { return m_start; }
  void start(GLfloat v){ m_start = v; }

  GLfloat end() const { return m_end; }
  void end(GLfloat v){ m_end = v; }

private:

  virtual void build_vertex();

  GLfloat m_radius = 1;
  GLfloat m_start = 0;
  GLfloat m_end = f2pi;
  GLuint m_slice = 16;
  CIRCLE_TYPE m_type = FILL;

};

}


#endif /* GL_GLC_CIRCLE_H */
