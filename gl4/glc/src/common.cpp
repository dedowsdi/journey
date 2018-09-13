#include "common.h"
#include <fstream>
#include <sstream>
#include "glm.h"
#include "glenumstring.h"

using namespace glm;

namespace zxd {

define_gl_type_traits(GLint, GL_INT);
define_gl_type_traits(GLfloat, GL_FLOAT);
define_gl_type_traits(GLdouble, GL_DOUBLE);
define_gl_type_traits(GLboolean, GL_BOOL);

//--------------------------------------------------------------------
void uniform_location(GLint *loc, GLint program, const std::string &name) {
  *loc = glGetUniformLocation(program, name.c_str());
  if (*loc == -1) {
    printf("failed to get uniform location : %s\n", name.c_str());
  }
}

//--------------------------------------------------------------------
std::string read_file(const std::string &filepath) {
  std::ifstream ifs(filepath);
  if (!ifs) {
    std::stringstream ss;
    ss << "failed to open file " << filepath << std::endl;
    throw std::runtime_error(ss.str());
  }

  // approximate size
  ifs.seekg(std::ios::end);
  GLuint size = ifs.tellg();
  ifs.seekg(std::ios::beg);

  std::string s;
  s.reserve(size);

  std::copy(std::istreambuf_iterator<char>(ifs),
    std::istreambuf_iterator<char>(), std::back_inserter(s));

  return s;
}

//--------------------------------------------------------------------
void matrix_attrib_pointer(
  GLint index, GLuint divisor /* = 1*/, GLboolean normalize /* = GL_FALSE*/) {
  glVertexAttribPointer(
    index + 0, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), BUFFER_OFFSET(0));
  glVertexAttribPointer(
    index + 1, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), BUFFER_OFFSET(16));
  glVertexAttribPointer(
    index + 2, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), BUFFER_OFFSET(32));
  glVertexAttribPointer(
    index + 3, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), BUFFER_OFFSET(48));

  glEnableVertexAttribArray(index + 0);
  glEnableVertexAttribArray(index + 1);
  glEnableVertexAttribArray(index + 2);
  glEnableVertexAttribArray(index + 3);

#ifndef GL_VERSION_3_0
  glVertexAttribDivisorARB(index + 0, divisor);
  glVertexAttribDivisorARB(index + 1, divisor);
  glVertexAttribDivisorARB(index + 2, divisor);
  glVertexAttribDivisorARB(index + 3, divisor);
#else
  glVertexAttribDivisor(index + 0, divisor);
  glVertexAttribDivisor(index + 1, divisor);
  glVertexAttribDivisor(index + 2, divisor);
  glVertexAttribDivisor(index + 3, divisor);
#endif
}

//--------------------------------------------------------------------
std::vector<GLubyte> create_chess_image(GLuint width, GLuint height,
  GLuint grid_width, GLuint grid_height,
  const glm::vec4 &black /* = glm::vec4(0, 0, 0, 1)*/,
  const glm::vec4 &white /* = glm::vec4(1)*/) {
  std::vector<GLubyte> image;
  image.reserve(width * height * 4);

  for (int i = 0; i < height; ++i) {
    for (int j = 0; j < width; ++j) {
      if ((i & grid_height) ^ (j & grid_width)) {
        image.push_back(black.r * 255);
        image.push_back(black.g * 255);
        image.push_back(black.b * 255);
        image.push_back(black.a * 255);
      } else {
        image.push_back(white.r * 255);
        image.push_back(white.g * 255);
        image.push_back(white.b * 255);
        image.push_back(white.a * 255);
      }
    }
  }

  return image;
}
//--------------------------------------------------------------------
void loadgl() {
#ifndef CLANG_COMPLETE_ONLY
  if (!gladLoadGL()) {
    printf("glad failed to load gl");
    return;
  }

  if (GLVersion.major < 2) {
    printf("your system doesn't support OpenGL >= 2!\n");
  }
#endif

  print_gl_version();
  init_debug_output();
}

//--------------------------------------------------------------------
void init_debug_output() {
  glEnable(GL_DEBUG_OUTPUT);
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

  glDebugMessageCallback(glDebugOutput, 0);
  // disable notification
  glDebugMessageControl(
    GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, 0, GL_FALSE);
}

//--------------------------------------------------------------------
void glDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity,
  GLsizei length, const GLchar *message, const void *user_param) {
  const char *debug_source = gl_debug_source_to_string(source);
  const char *debug_type = gl_debug_type_to_string(type);
  const char *debug_severity = gl_debug_severity_to_string(severity);

  printf("%s : %s : %s : %d : %.*s\n", debug_severity, debug_source, debug_type,
    id, length, message);
}

void print_gl_version() {
  printf(
    "GL_VERSION : %s\n"
    "GL_SHADING_LANGUAGE_VERSION : %s\n"
    "GL_RENDERER : %s\n"
    "GL_VENDOR : %s\n"
    "GL_SHADING_LANGUAGE_VERSION : %s\n"
#ifndef GL_VERSION_3_0
    "GLU_VERSION : %s\n"
#endif
    ,
    glGetString(GL_VERSION), glGetString(GL_SHADING_LANGUAGE_VERSION),
    glGetString(GL_RENDERER), glGetString(GL_VENDOR),
    glGetString(GL_SHADING_LANGUAGE_VERSION)
#ifndef GL_VERSION_3_0
      ,
    gluGetString(GLU_VERSION)
#endif
      );
}
}
