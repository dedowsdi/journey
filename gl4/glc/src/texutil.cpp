#include "texutil.h"

#include <iostream>

#include "stream_util.h"

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

  std::cout << "load image:" << file << " width:" << fip.getWidth() << ", height:" << fip.getHeight()
            << " bpp:" << fip.getBitsPerPixel() << std::endl;

  return fip;
}

//--------------------------------------------------------------------
fipImage fipLoadImage32(const std::string& file)
{
  fipImage fip = fipLoadImage(file);
  int bpp = fip.getBitsPerPixel();
  if(bpp != 32)
  {
    std::cout << "convert " << file << " to 32 bits" << std::endl;
    if(!fip.convertTo32Bits())
      std::cout << "convert failed!!" << std::endl;
  }
  return fip;
}

//--------------------------------------------------------------------
fipImage fipLoadResource(const std::string& file)
{
  return fipLoadImage(stream_util::get_resource(file));
}

//--------------------------------------------------------------------
fipImage fipLoadResource32(const std::string& file)
{
  return fipLoadImage32(stream_util::get_resource(file));
}

//--------------------------------------------------------------------
void save_texture(const std::string& filename, GLenum target, GLuint level,
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
