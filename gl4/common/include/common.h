#ifndef COMMON_H
#define COMMON_H
#include "glad/glad.h"
#include <string>
#include <iterator>
#include "glm.h"

#define BUFFER_OFFSET(bytes) ((GLubyte *)NULL + (bytes))

namespace zxd {

void setUniformLocation(GLint *loc, GLint program, const std::string &name);

std::string readFile(const std::string &filepath);

void matrixAttribPointer(
  GLint index, GLuint divisor = 1, GLboolean normalize = GL_FALSE);

template <typename T>
struct GL_type_traits {
  static GLenum gltype;
};

#define define_gl_type_traits(type, value) \
  template <>                              \
  GLenum GL_type_traits<type>::gltype = value;

// only works with seperated vertex data
//_It::valuetype has to be glm::vec*
template <typename _It>
void setupVertexAttrib(GLint attrib, _It beg, _It end,
  GLenum usage = GL_STATIC_DRAW, GLboolean normalize = GL_FALSE) {
  typedef typename std::iterator_traits<_It>::value_type tvec;

  GLuint vbo;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(tvec) * std::distance(beg, end),
    glm::value_ptr(*beg), usage);
  glVertexAttribPointer(attrib, tvec::components,
    GL_type_traits<typename tvec::value_type>::gltype, normalize, 0,
    BUFFER_OFFSET(0));

  glEnableVertexAttribArray(attrib);
}


template<typename T>
void setupVertexAttrib(GLint attrib, const T& t){
  setupVertexAttrib(attrib, t.begin(), t.end());
}


#define setupVertexAttribBuiltinArray(attrib, array) \
  setupVertexAttrib(                          \
    attrib, array, array + sizeof(array) / sizeof(decltype(array[0])));

}

#endif /* COMMON_H */
