#include "common.h"
#include <fstream>
#include <sstream>
#include "glm.h"

using namespace glm;

namespace zxd {

define_gl_type_traits(GLint, GL_INT);
define_gl_type_traits(GLfloat, GL_FLOAT);
define_gl_type_traits(GLdouble, GL_DOUBLE);
define_gl_type_traits(GLboolean, GL_BOOL);

//--------------------------------------------------------------------
void setUniformLocation(GLint *loc, GLint program, const std::string &name) {
  *loc = glGetUniformLocation(program, name.c_str());
  if (*loc == -1) {
    printf("failed to get uniform location : %s\n", name.c_str());
  }
}

//--------------------------------------------------------------------
std::string readFile(const std::string &filepath) {
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
void matrixAttribPointer(
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

  glVertexAttribDivisor(index + 0, 1);
  glVertexAttribDivisor(index + 1, 1);
  glVertexAttribDivisor(index + 2, 1);
  glVertexAttribDivisor(index + 3, 1);
}

//--------------------------------------------------------------------
std::vector<GLubyte> createChessImage(GLuint width, GLuint height, GLuint gridWidth,
  GLuint gridHeight, const glm::vec4 &black /* = glm::vec4(0, 0, 0, 1)*/,
  const glm::vec4 &white /* = glm::vec4(1)*/) {
  std::vector<GLubyte> image;
  image.reserve(width * height * 4);

  for (int i = 0; i < height; ++i) {
    for (int j = 0; j < width; ++j) {
      if ((i & gridHeight) ^ (j & gridWidth)) {
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
}
