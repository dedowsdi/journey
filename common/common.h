#ifndef COMMON_H
#define COMMON_H

#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include <GL/glu.h>
#include "string.h"
#include <algorithm>
#include <iostream>

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

const GLchar *getVersions();

const GLchar *getExtensions();

// you must free the string by your self
GLchar *readFile(const char *file);

GLboolean queryExtension(char *extName);

GLboolean invertMatrixd(GLdouble m[16]);

GLenum rotateEnum(GLenum val, GLenum begin, GLuint size);

// c ctyle attach
void attachShaderFile(GLuint prog, GLenum type, char *file);
void attachShaderSourceAndFile(
  GLuint prog, GLenum type, GLuint count, char **source, char *file);
bool attachShaderSource(GLuint prog, GLenum type, GLuint count, char **source);
// exit if not found
void setUniformLocation(GLint *loc, GLint program, char *name);
void setUniformLocation(GLint *loc, GLint program, const std::string& name);

GLfloat getTime();
GLfloat getNormalizedTime();
// only work if get called exactly once per frame
GLfloat updateFps();

void initExtension();
void initDebugOutput();
void glDebugOutput(GLenum source, GLenum type, GLuint id,
  GLenum severity, GLsizei length, const GLchar *message, const void *userParam);

void drawTexRect(GLfloat x0, GLfloat y0, GLfloat x1, GLfloat y1);

void drawXZRect(GLfloat x0, GLfloat z0, GLfloat x1, GLfloat z1);

void drawXYPlane(
  GLfloat x0, GLfloat y0, GLfloat x1, GLfloat y1, GLuint slices = 1);

void detachAllShaders(GLuint program);

void getModelViewProj(GLfloat *p);

// print gigantic array
template <typename _It>
void printArray(_It beg, _It end, unsigned itemsPerLine) {
  int i = 0;
  std::for_each(beg, end, [&](decltype(*beg) v) {
    std::cout << v << ", ";
    if (++i % itemsPerLine == 0) {
      std::cout << std::endl;
    }
  });
}

#endif /* COMMON_H */
