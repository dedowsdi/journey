#ifndef GL4_GLC_ARRAY_H
#define GL4_GLC_ARRAY_H

#include <memory>

#include <glm.h>
#include <mixinvector.h>
#include <glm_traits.h>

#ifndef BUFFER_OFFSET
#define BUFFER_OFFSET(bytes) ((GLubyte*)NULL + (bytes))
#endif

namespace zxd
{

using namespace glm;

class array
{
public:
  virtual ~array() = default;

  virtual GLuint size() const = 0;

  virtual GLuint element_bytes() const = 0;

  virtual GLuint bytes() const = 0;

  virtual GLuint element_size() const = 0; // same as vertex attrib size

  virtual GLenum gltype() const = 0; // same as vertex attrib type

  virtual const void* data() const = 0;

};

template <typename T>
class template_array : public array, public mixin_vector<T>
{
public:

  using mixin_vector<T>::mixin_vector;

  GLuint size() const override { return mixin_vector<T>::size(); }

  virtual GLuint element_bytes() const override
  {
    return sizeof(mixin_vector<T>::front());
  };

  GLuint bytes() const override
  {
    return size() * sizeof(mixin_vector<T>::front());
  }

  GLuint element_size() const override { return glm_type_components<T>; }

  GLenum gltype() const override { return glm_type_enum<T>; }

  virtual const void* data() const override
  {
    return &mixin_vector<T>::front();
  };
};

using char_array = template_array<GLchar>;
using uint_array = template_array<uint>;
using ushort_array = template_array<ushort>;
using uvec1_array = template_array<uvec1>;
using vec1_array = template_array<vec1>;
using float_array = template_array<float>;
using vec2_array = template_array<vec2>;
using vec3_array = template_array<vec3>;
using vec4_array = template_array<vec4>;

using array_uptr = std::unique_ptr<array>;
using array_ptr = std::shared_ptr<array>;
using vec1_array_uptr = std::unique_ptr<vec1_array>;
using vec2_array_uptr = std::unique_ptr<vec2_array>;
using vec3_array_uptr = std::unique_ptr<vec3_array>;
using vec4_array_uptr = std::unique_ptr<vec4_array>;
using vec1_array_ptr = std::shared_ptr<vec1_array>;
using vec2_array_ptr = std::shared_ptr<vec2_array>;
using vec3_array_ptr = std::shared_ptr<vec3_array>;
using vec4_array_ptr = std::shared_ptr<vec4_array>;
}

#endif /* GL4_GLC_ARRAY_H */
