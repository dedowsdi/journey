#ifndef GL_GLC_ARRAY_H
#define GL_GLC_ARRAY_H
#include "mixinvector.h"
#include "glm.h"
#include <memory>
#include "common.h"

namespace zxd {

class array {
protected:
  GLuint m_buffer;
  GLenum m_target;
  GLenum m_usage;
  array(GLenum target = GL_ARRAY_BUFFER, GLenum usage = GL_STATIC_DRAW)
    : m_buffer(-1),
    m_target(target),
    m_usage(usage)
  {}
  
  virtual ~array() {}
  void bind_buffer() {
    if (m_buffer == -1) {
      glGenBuffers(1, &m_buffer);
    }
    glBindBuffer(m_target, m_buffer);
  }

public:
  virtual void update_buffer() {}
  virtual void bind(GLint location) {}
  virtual GLuint num_elements() const = 0;

  GLenum target() const { return m_target; }
  void target(GLenum v){ m_target = v; }

  GLenum usage() const { return m_usage; }
  void usage(GLenum v){ m_usage = v; }
};

template <typename T>
class template_array : public array, public mixin_vector<T> {

public:
  template_array(GLenum target = GL_ARRAY_BUFFER, GLenum usage = GL_STATIC_DRAW);
  void bind(GLint location);
  void update_buffer();
  virtual GLuint num_elements() const { return this->size(); }
};

typedef template_array<float> float_array;
typedef template_array<vec2> vec2_array;
typedef template_array<vec3> vec3_array;
typedef template_array<vec4> vec4_array;
typedef template_array<GLuint> uint_array;
typedef template_array<GLushort> ushort_array;

typedef std::shared_ptr<array> array_ptr;
typedef std::shared_ptr<float_array> float_array_ptr;
typedef std::shared_ptr<vec2_array> vec2_array_ptr;
typedef std::shared_ptr<vec3_array> vec3_array_ptr;
typedef std::shared_ptr<vec4_array> vec4_array_ptr;

//--------------------------------------------------------------------
template<typename T>
template_array<T>::template_array(GLenum target/* = GL_ARRAY_BUFFER*/, GLenum usage/* = GL__DRAW*/):
  array(target, usage)
{
}

//--------------------------------------------------------------------
template <typename T>
void template_array<T>::bind(GLint location) {
  bind_buffer();
  glVertexAttribPointer(location, glm_vecn<T>::components,
    gl_type_traits<typename glm_vecn<T>::value_type>::gltype, GL_FALSE, 0,
    BUFFER_OFFSET(0));
  glEnableVertexAttribArray(location);
}

//--------------------------------------------------------------------
template <typename T>
void template_array<T>::update_buffer() {
  bind_buffer();
  glBufferData(m_target, this->size() * sizeof(decltype(this->front())),
    &this->front(), m_usage);
}
}

#endif /* GL_GLC_ARRAY_H */
