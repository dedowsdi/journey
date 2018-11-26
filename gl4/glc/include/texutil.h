#ifndef GL_GLC_TEXUTIL_H
#define GL_GLC_TEXUTIL_H
#include "gl.h"
#include <string>
#include <FreeImagePlus.h>

namespace zxd {
fipImage fipLoadImage(const std::string& file);
fipImage fipLoadImage32(const std::string& file);
fipImage fipLoadResource(const std::string& file);
fipImage fipLoadResource32(const std::string& file);

void save_texture(const std::string& filename, GLenum target, GLuint level,
  GLenum format, GLenum type, GLuint size);
}

#endif /* GL_GLC_TEXUTIL_H */
