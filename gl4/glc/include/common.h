#ifndef GL_GLC_COMMON_H
#define GL_GLC_COMMON_H
#include "gl.h"
#ifndef GL_VERSION_3_0
//#include <GL/glu.h>
#endif
#include <string>
#include <vector>
#include <iterator>

#include "glm.h"
#include <glm/gtc/type_ptr.hpp>

#ifndef BUFFER_OFFSET
#define BUFFER_OFFSET(bytes) ((GLubyte *)NULL + (bytes))
#endif

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

namespace zxd
{

GLint get_uniform_location(GLint program, const std::string &name);

void matrix_attrib_pointer(
  GLint index, GLuint divisor = 1, GLboolean normalize = GL_FALSE);

// handle difference between float and vec
template <typename T>
struct glm_vecn
{
  static const GLuint components = T::components;
  typedef typename T::value_type value_type;
};

template <>
struct glm_vecn<GLfloat>
{
  static const GLuint components = 1;
  typedef GLfloat value_type;
};


template <>
struct glm_vecn<GLuint>
{
  static const GLuint components = 1;
  typedef GLuint value_type;
};

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

#endif /* GL_GLC_COMMON_H */
