#ifndef TEXUTIL_H
#define TEXUTIL_H
#include "glad/glad.h"
#include <string>
#include <FreeImagePlus.h>

namespace zxd {
fipImage fipLoadImage(const std::string& file);

void save_texture(const std::string& filename, GLenum target, GLuint level,
  GLenum format, GLenum type, GLuint size);
}

#endif /* TEXUTIL_H */
