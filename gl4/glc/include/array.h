#ifndef ARRAY
#define ARRAY
#include "mixinvector.h"
#include "glm.h"
#include <memory>
#include "common.h"

namespace zxd {

class array {
protected:
  GLuint m_buffer;
  array() : m_buffer(-1) {}
  virtual ~array() {}
  void bind_array_buffer() {
    if (m_buffer == -1) {
      glGenBuffers(1, &m_buffer);
    }
    glBindBuffer(GL_ARRAY_BUFFER, m_buffer);
  }

public:
  virtual void update_array_buffer() {}
  virtual void bind(GLint location) {}
  virtual GLuint num_elements() const = 0;
};

template <typename T>
class template_array : public array, public mixin_vector<T> {

public:
  void bind(GLint location);
  void update_array_buffer();
  virtual GLuint num_elements() const { return this->size(); }
};

typedef template_array<float> float_array;
typedef template_array<vec2> vec2_array;
typedef template_array<vec3> vec3_array;
typedef template_array<vec4> vec4_array;

typedef std::shared_ptr<array> array_ptr;
typedef std::shared_ptr<float_array> float_array_ptr;
typedef std::shared_ptr<vec2_array> vec2_array_ptr;
typedef std::shared_ptr<vec3_array> vec3_array_ptr;
typedef std::shared_ptr<vec4_array> vec4_array_ptr;

//--------------------------------------------------------------------
template <typename T>
void template_array<T>::bind(GLint location) {
  bind_array_buffer();
  glVertexAttribPointer(location, glm_vecn<T>::components,
    gl_type_traits<typename glm_vecn<T>::value_type>::gltype, GL_FALSE, 0,
    BUFFER_OFFSET(0));
  glEnableVertexAttribArray(location);
}

//--------------------------------------------------------------------
template <typename T>
void template_array<T>::update_array_buffer() {
  bind_array_buffer();
  glBufferData(GL_ARRAY_BUFFER, this->size() * sizeof(decltype(this->front())),
    &this->front(), GL_STATIC_DRAW);
}
}

#endif /* ARRAY */
