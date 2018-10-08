#ifndef PRIMITIVE_SET_H
#define PRIMITIVE_SET_H
#include "gl.h"
#include <memory>
#include <vector>

namespace zxd
{

class primitive_set
{
protected:
  GLenum m_mode;
  GLsizei m_num_instance;

public:

  primitive_set(GLenum mode, GLuint num_instance);
  virtual ~primitive_set(){}
  virtual void draw() = 0;

  GLenum mode() const { return m_mode; }
  void mode(GLenum v){ m_mode = v; }

  GLsizei num_instance() const { return m_num_instance; }
  void num_instance(GLsizei v){ m_num_instance = v; }
};

typedef std::vector<std::shared_ptr<primitive_set>>  primitive_set_vector;

class draw_arrays : public primitive_set
{
protected:
  GLint m_first;
  GLsizei m_count;

public:
  draw_arrays(GLenum mode, GLint first, GLsizei count, GLuint num_instance = 0);
  virtual void draw();

  GLint first() const { return m_first; }
  void first(GLint v){ m_first = v; }

  GLsizei count() const { return m_count; }
  void count(GLsizei v){ m_count = v; }
};

}


#endif /* PRIMITIVE_SET_H */
