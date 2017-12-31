#ifndef TEXUTIL_H
#define TEXUTIL_H
#include <GL/glew.h>
#include <string>
#include <FreeImagePlus.h>

namespace zxd {
fipImage fipLoadImage(const std::string& file);

void saveTexture(const std::string& filename, GLenum target, GLuint level,
  GLenum format, GLenum type, GLuint size);
}

#endif /* TEXUTIL_H */
