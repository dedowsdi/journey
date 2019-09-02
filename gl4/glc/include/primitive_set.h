#ifndef GL_GLC_PRIMITIVE_SET_H
#define GL_GLC_PRIMITIVE_SET_H
#include "glm.h"

namespace zxd
{

using namespace glm;

// changed from osg
class primitive_functor
{
public:

  virtual void set_vertex_array(unsigned int count, const vec3* vertices) = 0;

  virtual ~primitive_functor() {}

  virtual void drawElements(GLenum mode,GLsizei count,const GLuint* indices){};
  virtual void drawArrays(GLenum mode,GLint first,GLsizei count){};

  //void useVertexCacheAsVertexArray()
  //{
  //setVertexArray(m_vertex_cache.size(),&m_vertex_cache.front());
  //}

  //std::vector<glm::vec3>   m_vertex_cache;
  bool m_treat_vertex_data_as_temporary;
};

class primitive_index_functor
{
public:
    virtual ~primitive_index_functor() {}

    virtual void setVertexArray(unsigned int count,const vec3* vertices) = 0;

    virtual void drawElements(GLenum mode,GLsizei count,const GLuint* indices) = 0;

    virtual void begin(GLenum mode) = 0;
    virtual void vertex(unsigned int pos) = 0;
    virtual void end() = 0;

    //void useVertexCacheAsVertexArray()
    //{
        //setVertexArray(_vertexCache.size(),&_vertexCache.front());
    //}
    //std::vector<Vec3>   _vertexCache;
    bool                _treatVertexDataAsTemporary;
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

  virtual void accept(primitive_functor& functor) const{};
  virtual void accept(primitive_index_functor& functor) const{};
};


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

class draw_elements : public primitive_set
{
protected:
  GLint m_offset;
  GLenum m_type;
  GLsizei m_count;
  const GLuint* m_indices;

public:
  draw_elements(GLenum mode, GLint count, GLenum type, GLint offset, GLuint num_instance = 0);
  virtual void draw();

  GLenum type() const { return m_type; }
  void type(GLenum v){ m_type = v; }

  GLsizei count() const { return m_count; }
  void count(GLsizei v){ m_count = v; }

  GLint offset() const { return m_offset; }
  void offset(GLint v){ m_offset = v; }

  const GLuint* indices() const { return m_indices; }
  void indices(const GLuint* v){ m_indices = v; }

  virtual void accept(primitive_functor& functor) const;
};

}

#endif /* GL_GLC_PRIMITIVE_SET_H */
