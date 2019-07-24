#ifndef GL_GLC_ARRAY_H
#define GL_GLC_ARRAY_H
#include <memory>
#include <iostream>

#include "glm.h"
#include "mixinvector.h"

#define BUFFER_OFFSET(bytes) ((GLubyte *)NULL + (bytes))

namespace zxd
{

using namespace glm;

class array
{
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
  void bind_buffer()
  {
    if (m_buffer == -1)
    {
      glGenBuffers(1, &m_buffer);
    }
    glBindBuffer(m_target, m_buffer);
  }

public:
  virtual void update_buffer() {}
  virtual void bind(GLint location) {}
  virtual GLuint num_elements() const = 0;
  virtual void read_buffer() = 0;

  GLenum target() const { return m_target; }
  void target(GLenum v){ m_target = v; }

  GLenum usage() const { return m_usage; }
  void usage(GLenum v){ m_usage = v; }

  GLuint buffer() const { return m_buffer; }
};

template <typename T>
class template_array : public array, public mixin_vector<T>
{

public:
  template_array(GLenum target = GL_ARRAY_BUFFER, GLenum usage = GL_STATIC_DRAW);
  void bind(GLint location);
  void update_buffer();
  virtual GLuint num_elements() const { return this->size(); }
  virtual void read_buffer();
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

//template <typename T> constexpr GLenum gl_type_enum = GL_FLOAT;
//template<> constexpr GLenum gl_type_enum<GLchar> = GL_BYTE;
//template<> constexpr GLenum gl_type_enum<GLubyte> = GL_UNSIGNED_BYTE;
//template<> constexpr GLenum gl_type_enum<GLshort> = GL_SHORT;
//template<> constexpr GLenum gl_type_enum<GLushort> = GL_UNSIGNED_SHORT;
//template<> constexpr GLenum gl_type_enum<GLint> = GL_INT;
//template<> constexpr GLenum gl_type_enum<GLuint> = GL_UNSIGNED_INT;
//template<> constexpr GLenum gl_type_enum<GLdouble> = GL_DOUBLE;
// partial specialization cause odr problem in gcc 5.4.0, why?
//template<typename T, precision P> constexpr GLenum gl_type_enum<tvec2<T, P>> = gl_type_enum<T>;
//template<typename T, precision P> constexpr GLenum gl_type_enum<tvec3<T, P>> = gl_type_enum<T>;
//template<typename T, precision P> constexpr GLenum gl_type_enum<tvec4<T, P>> = gl_type_enum<T>;
//template<> constexpr GLenum gl_type_enum<GLboolean> = GL_BOOL; // the same as GLubyte

template <typename T> 
struct gl_type_enum {static constexpr GLuint value = GL_FLOAT;};

template<> struct gl_type_enum<GLchar> {static constexpr GLuint value = GL_BYTE;};
template<> struct gl_type_enum<GLubyte> {static constexpr GLuint value = GL_UNSIGNED_BYTE;};
template<> struct gl_type_enum<GLshort> {static constexpr GLuint value = GL_SHORT;};
template<> struct gl_type_enum<GLushort> {static constexpr GLuint value = GL_UNSIGNED_SHORT;};
template<> struct gl_type_enum<GLint> {static constexpr GLuint value = GL_INT;};
template<> struct gl_type_enum<GLuint> {static constexpr GLuint value = GL_UNSIGNED_INT;};
template<> struct gl_type_enum<GLdouble> {static constexpr GLuint value = GL_DOUBLE;};
template<typename T, precision P>
struct gl_type_enum<tvec2<T, P>> {static constexpr GLuint value = gl_type_enum<T>::value;};
template<typename T, precision P>
struct gl_type_enum<tvec3<T, P>> {static constexpr GLuint value = gl_type_enum<T>::value;};
template<typename T, precision P>
struct gl_type_enum<tvec4<T, P>> {static constexpr GLuint value = gl_type_enum<T>::value;};

// partial specialization cause odr problem, but why?
//template<typename T, precision P> constexpr GLenum gl_type_enum<tvec2<T, P>> = gl_type_enum<T>;
//template<typename T, precision P> constexpr GLenum gl_type_enum<tvec3<T, P>> = gl_type_enum<T>;
//template<typename T, precision P> constexpr GLenum gl_type_enum<tvec4<T, P>> = gl_type_enum<T>;
//template<> constexpr GLenum gl_type_enum<GLboolean> = GL_BOOL; // the same as GLubyte

// following code violate odr in gcc 5.4.0
//template <typename T> constexpr GLuint glm_type_components = 1;
//template <typename T, precision P> GLuint glm_type_components<tvec2<T,P>> = 2;
//template <typename T, precision P> GLuint glm_type_components<tvec3<T,P>> = 3;
//template <typename T, precision P> GLuint glm_type_components<tvec4<T,P>> = 4;

template <typename T>
struct glm_type_components { static constexpr GLuint value = 1; };

template <typename T, precision P>
struct glm_type_components<tvec2<T, P>> { static constexpr GLuint value = 2; };

template <typename T, precision P>
struct glm_type_components<tvec3<T, P>> { static constexpr GLuint value = 3; };

template <typename T, precision P>
struct glm_type_components<tvec4<T, P>> { static constexpr GLuint value = 4; };

//--------------------------------------------------------------------
template<typename T>
template_array<T>::template_array(GLenum target/* = GL_ARRAY_BUFFER*/, GLenum usage/* = GL__DRAW*/):
  array(target, usage)
{
}

//--------------------------------------------------------------------
template <typename T>
void template_array<T>::bind(GLint location)
{
  bind_buffer();
  glVertexAttribPointer(location, glm_type_components<T>::value, gl_type_enum<T>::value,
      GL_FALSE, 0, BUFFER_OFFSET(0));
  glEnableVertexAttribArray(location);
}

//--------------------------------------------------------------------
template <typename T>
void template_array<T>::update_buffer()
{
  bind_buffer();
  glBufferData(m_target, this->size() * sizeof(decltype(this->front())),
    &this->front(), m_usage);
}

//--------------------------------------------------------------------
template<typename T>
void template_array<T>::read_buffer()
{
  bind_buffer();
  GLint buffer_size;
  glGetBufferParameteriv(m_target, GL_BUFFER_SIZE, &buffer_size);
  glGetBufferSubData(m_target, 0, buffer_size, &this->get_vector().front());
}
}

#endif /* GL_GLC_ARRAY_H */
