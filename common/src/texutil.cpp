#include "texutil.h"
#include <iostream>

namespace zxd {

//--------------------------------------------------------------------
fipImage fipLoadImage(const std::string& file) {
  fipImage fip;
  FREE_IMAGE_FORMAT fif = fipImage::identifyFIF(file.c_str());
  if (fif == FIF_UNKNOWN) {
    std::cerr << "unknown file " << file << std::endl;
  }

  if (!fip.load(file.c_str())) {
    std::cerr << "error : failed to load " << file << std::endl;
  }

  // fip.flipVertical();

  return fip;
}

//--------------------------------------------------------------------
void saveTexture(const std::string& filename, GLenum target, GLuint level,
  GLenum format, GLenum type, GLuint size) {
  GLint width, height;

  glGetTexLevelParameteriv(target, level, GL_TEXTURE_WIDTH, &width);
  glGetTexLevelParameteriv(target, level, GL_TEXTURE_HEIGHT, &height);

  fipImage fip;
  fip.setSize(FIT_BITMAP, width, height, size);

  GLubyte* pixel = fip.accessPixels();
  glGetTexImage(target, level, format, type, pixel);

  fip.save(filename.c_str());
}
}
