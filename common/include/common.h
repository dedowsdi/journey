#ifndef COMMON_H
#define COMMON_H

#include <glad/glad.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include "string.h"
#include <algorithm>
#include <iostream>

typedef std::vector<std::string> StringVector;
typedef std::vector<const char *> CStringVector;

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

void printGLVersion();

const GLchar *getExtensions();

std::string readFile(const std::string &filepath);

GLboolean queryExtension(char *extName);

GLboolean invertMatrixd(GLdouble m[16]);

GLenum rotateEnum(GLenum val, GLenum begin, GLuint size);

// c ctyle attach
void attachShaderFile(GLuint prog, GLenum type, const std::string &file);
// combine source and file
void attachShaderSourceAndFile(
  GLuint prog, GLenum type, StringVector &source, const std::string &file);
bool attachShaderSource(GLuint prog, GLenum type, const StringVector &source);
bool attachShaderSource(GLuint prog, GLenum type, const std::string& source);
// exit if not found
void setUniformLocation(GLint *loc, GLint program, char *name);
void setUniformLocation(GLint *loc, GLint program, const std::string &name);

GLfloat getTime();
GLfloat getNormalizedTime();
// only work if get called exactly once per frame
GLfloat updateFps();

void loadGL();
void initDebugOutput();
void glDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity,
  GLsizei length, const GLchar *message, const void *userParam);

void drawTexRect(GLfloat x0, GLfloat y0, GLfloat x1, GLfloat y1);

void drawXZRect(GLfloat x0, GLfloat z0, GLfloat x1, GLfloat z1);

void drawXYPlane(
  GLfloat x0, GLfloat y0, GLfloat x1, GLfloat y1, GLuint slices = 1);

void detachAllShaders(GLuint program);

void getModelViewProj(GLfloat *p);

// only works with array buffer
void matrixAttribPointer(GLint index, GLuint divisor = 1,  GLboolean normalize = GL_FALSE);

GLint getAttribLocation(GLuint program, const std::string& name);

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
