#include "texture.h"

#include <iostream>
#include <algorithm>

#include <texutil.h>

namespace zxd
{

//--------------------------------------------------------------------
image::image(const std::string& filename):
  _filename(filename)
{
  if(!filename.empty())
    load(filename);
}

//--------------------------------------------------------------------
void image::load(const std::string& filename)
{
  _filename = filename;
  auto img = fipLoadResource32(filename);
  _width = img.getWidth();
  _height = img.getHeight();
  auto bytes = _width * _height * 4;
  _data.clear();
  _data.resize(bytes);
  _data.shrink_to_fit();

  std::copy(img.accessPixels(), img.accessPixels() + bytes, _data.begin());
}

//--------------------------------------------------------------------
const char* image::get_data() const
{
  return &_data.front();
}

//--------------------------------------------------------------------
char* image::get_data()
{
  return &_data.front();
}

//--------------------------------------------------------------------
void image::set_data(std::vector<GLchar>&& data, GLuint width, GLuint height)
{
  _data = std::move(data);
  _width = width;
  _height = height;
  _filename.clear();
}

//--------------------------------------------------------------------
void image::set_data(const std::vector<GLchar>& data, GLuint width, GLuint height)
{
  auto d = data;
  set_data(std::move(d), width, height);
}

//--------------------------------------------------------------------
texture::texture()
{
  glGenTextures(1, &_object);
}

//--------------------------------------------------------------------
texture::~texture()
{
  if(_object != 0)
  {
    glDeleteTextures(1, &_object);
    std::cout << "delete texture : " << _object << std::endl;
  }
}

//--------------------------------------------------------------------
void texture::bind(GLenum target)
{
  glBindTexture(target, _object);
  _target = target;
}

//--------------------------------------------------------------------
void texture::generate_mipmap()
{
  bind(_target);
  glGenerateMipmap(_target);
}

//--------------------------------------------------------------------
void texture::set_img(const std::shared_ptr<image>& v,
  GLenum internal_format, GLenum format,
  GLenum type)
{
  _img = v;
  bind(GL_TEXTURE_2D);
  glTexImage2D(GL_TEXTURE_2D, 0, internal_format, v->get_width(),
    v->get_height(), 0, format, type, _img->get_data());
}

}
