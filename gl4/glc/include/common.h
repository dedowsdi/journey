#ifndef COMMON_H
#define COMMON_H
#include "glad/glad.h"
#ifndef GL_VERSION_3_0
#include <GL/glu.h>
#endif
#include <string>
#include <iterator>
#include "glm.h"

#define BUFFER_OFFSET(bytes) ((GLubyte *)NULL + (bytes))

#define ENABLE_GL_CHECK 0

#if ENABLE_GL_CHECK
#define ZXD_CHECK_GL_ERROR(glFunc)                                             \
  {                                                                            \
    int e = glGetError();                                                      \
    if (e != 0) {                                                              \
      printf("missing openGL error %x %s in %s before %s:line %d for %s\n", e, \
        gluErrorString(e), __PRETTY_FUNCTION__, __FILE__, __LINE__, #glFunc);  \
    }                                                                          \
    glFunc;                                                                    \
    e = glGetError();                                                          \
    if (e != 0) {                                                              \
      printf("found openGL error %x %s in %s at %s:line %d for %s\n", e,       \
        gluErrorString(e), __PRETTY_FUNCTION__, __FILE__, __LINE__, #glFunc);  \
    }                                                                          \
  }

#define ZXD_CHECK_GL_ERROR_AFTER                                        \
  {                                                                     \
    int e = glGetError();                                               \
    if (e != 0) {                                                       \
      printf("missing openGL error %x %s in %s before %s:line %d\n", e, \
        gluErrorString(e), __PRETTY_FUNCTION__, __FILE__, __LINE__);    \
    }                                                                   \
  }
#else
#define ZXD_CHECK_GL_ERROR(glFunc) \
  { glFunc; }
#define ZXD_CHECK_GL_ERROR_AFTER
#endif

#define ZCGE ZXD_CHECK_GL_ERROR
#define ZCGEA ZXD_CHECK_GL_ERROR_AFTER

namespace zxd {

void uniform_location(GLint *loc, GLint program, const std::string &name);

std::string read_file(const std::string &filepath);

void matrix_attrib_pointer(
  GLint index, GLuint divisor = 1, GLboolean normalize = GL_FALSE);

template <typename T>
struct gl_type_traits {
  static GLenum gltype;
};

#define define_gl_type_traits(type, value) \
  template <>                              \
  GLenum gl_type_traits<type>::gltype = value;

// handle difference between float and vec
template <typename T>
struct glm_vecn {
  static const GLuint components = T::components;
  typedef typename T::value_type value_type;
};

template <>
struct glm_vecn<GLfloat> {
  static const GLuint components = 1;
  typedef GLfloat value_type;
};

// only works with seperated vertex data
//_It::valuetype has to be glm::vec*
template <typename _It>
void setup_vertex_attrib(GLint attrib, _It beg, _It end,
  GLenum usage = GL_STATIC_DRAW, GLboolean normalize = GL_FALSE) {
  typedef typename std::iterator_traits<_It>::value_type tvec;

  GLuint vbo;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(tvec) * std::distance(beg, end),
    glm::value_ptr(*beg), usage);
  glVertexAttribPointer(attrib, tvec::components,
    gl_type_traits<typename tvec::value_type>::gltype, normalize, 0,
    BUFFER_OFFSET(0));

  glEnableVertexAttribArray(attrib);
}

template <typename T>
void setup_vertex_attrib(GLint attrib, const T &t) {
  setup_vertex_attrib(attrib, t.begin(), t.end());
}

#define setup_vertex_al_builtin_array(attrib, array) \
  setup_vertex_attrib(                               \
    attrib, array, array + sizeof(array) / sizeof(decltype(array[0])));

std::vector<GLubyte> create_chess_image(GLuint width, GLuint height,
  GLuint grid_width, GLuint grid_height,
  const glm::vec4 &black = glm::vec4(0, 0, 0, 1),
  const glm::vec4 &white = glm::vec4(1));

void loadgl();
void init_debug_output();
void glDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity,
  GLsizei length, const GLchar *message, const void *user_param);
void print_gl_version();
}

#endif /* COMMON_H */
