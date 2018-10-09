#ifndef PRIMITIVE_SET_H
#define PRIMITIVE_SET_H
#include "gl.h"
#include <memory>
#include <vector>
#include "glm.h"

namespace zxd
{

// changed from osg
class primitive_functor
{
public:

  virtual void set_vertex_array(unsigned int count, const vec3* vertices) = 0;

  virtual ~primitive_functor() {}

  virtual void drawArrays(GLenum mode,GLint first,GLsizei count) = 0;

  //virtual void drawElements(GLenum mode,GLsizei count,const GLubyte* indices) = 0;
  //virtual void drawElements(GLenum mode,GLsizei count,const GLushort* indices) = 0;
  //virtual void drawElements(GLenum mode,GLsizei count,const GLuint* indices) = 0;

  //void useVertexCacheAsVertexArray()
  //{
  //setVertexArray(m_vertex_cache.size(),&m_vertex_cache.front());
  //}

  //std::vector<glm::vec3>   m_vertex_cache;
  bool m_treat_vertex_data_as_temporary;
};


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

  virtual void accept(primitive_functor& functor) const = 0;
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

  virtual void accept(primitive_functor& functor) const;
};

}


#endif /* PRIMITIVE_SET_H */
