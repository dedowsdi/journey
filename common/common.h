#ifndef COMMON_H
#define COMMON_H

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <stdio.h>
#include <GL/glu.h>
#include "string.h"

#define BUFFER_OFFSET(bytes) ((GLubyte*)NULL + (bytes))

#define ENABLE_GL_CHECK 1

#if ENABLE_GL_CHECK
#define ZXD_CHECK_GL_ERROR(glFunc)                                          \
  {                                                                         \
    int e = glGetError();                                                   \
    if (e != 0) {                                                           \
      printf("missing openGL error %x %s in %s before line %d for %s\n", e, \
        gluErrorString(e), __PRETTY_FUNCTION__, __LINE__, #glFunc);         \
    }                                                                       \
    glFunc;                                                                 \
    e = glGetError();                                                       \
    if (e != 0) {                                                           \
      printf("found openGL error %x %s in %s at line %d for %s\n", e,       \
        gluErrorString(e), __PRETTY_FUNCTION__, __LINE__, #glFunc);         \
    }                                                                       \
  }

#define ZXD_CHECK_GL_ERROR_AFTER                                     \
  {                                                                  \
    int e = glGetError();                                            \
    if (e != 0) {                                                    \
      printf("missing openGL error %x %s in %s before line %d\n", e, \
        gluErrorString(e), __PRETTY_FUNCTION__, __LINE__);           \
    }                                                                \
  }
#else
#define ZXD_CHECK_GL_ERROR(glFunc) \
  { glFunc; }
#define ZXD_CHECK_GL_ERROR_AFTER
#endif

#define ZCGE ZXD_CHECK_GL_ERROR
#define ZCGEA ZXD_CHECK_GL_ERROR_AFTER

const GLchar *getVersions() ;

const GLchar *getExtensions() ;

GLboolean queryExtension(char *extName) ;

GLboolean invertMatrixd(GLdouble m[16]) ;

GLenum rotateEnum(GLenum val, GLenum begin, GLuint size) ;

void attachShaderSource(GLuint prog, GLenum type, const char *source);
void attachShaderFile(GLuint prog, GLenum type, const char *source);

#endif /* COMMON_H */
