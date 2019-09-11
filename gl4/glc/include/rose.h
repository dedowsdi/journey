#ifndef GL_GLC_ROSE_H
#define GL_GLC_ROSE_H
#include <geometry.h>

namespace zxd
{

class rose : public common_geometry
{

public:

  rose(GLuint n = 1, GLuint d = 1);

  GLuint n() const { return m_n; }
  void n(GLuint v){ m_n = v; }

  GLuint d() const { return m_d; }
  void d(GLuint v){ m_d = v; }

  GLuint slice() const { return m_slice; }
  void slice(GLuint v){ m_slice = v; }

  GLfloat radius() const { return m_radius; }
  void radius(GLfloat v){ m_radius = v; }

  GLfloat offset() const { return m_offset; }
  void offset(GLfloat v){ m_offset = v; }

  GLfloat start() const { return m_start; }
  void start(GLfloat v){ m_start = v; }

  GLfloat end() const { return m_end; }
  void end(GLfloat v){ m_end = v; }

  static vec2 get_rose(
    GLfloat radius, GLfloat angle, GLuint n, GLuint d, GLfloat offset);

private:

  vertex_build build_vertices() override;

  GLuint m_n = 1;
  GLuint m_d = 1;
  GLuint m_slice = 32;
  GLfloat m_radius = 1;
  GLfloat m_offset = 0;
  GLfloat m_start = 0; // normalized
  GLfloat m_end = 1; // normalized

};

}

#endif /* GL_GLC_ROSE_H */
