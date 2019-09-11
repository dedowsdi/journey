#include <buffer.h>

#include <iostream>

#include <array.h>

namespace zxd
{

//--------------------------------------------------------------------
buffer::buffer()
{
  glGenBuffers(1, &_object);
}

//--------------------------------------------------------------------
buffer::~buffer()
{
  if(_object != 0)
  {
    glDeleteBuffers(1, &_object);
    std::cout << "buffer " << _object << " deleted"  << std::endl;
  }
}

//--------------------------------------------------------------------
void buffer::bind(GLenum target)
{
  glBindBuffer(target, _object);
  _target = target;
}

//--------------------------------------------------------------------
void buffer::buffer_data(GLsizei size, const GLvoid* data, GLenum usage)
{
  bind(_target);
  glBufferData(_target, size, data, usage);
  _size = size;
  _usage = usage;
}

//--------------------------------------------------------------------
void buffer::buffer_data(std::unique_ptr<array> data, GLenum usage)
{
  glBufferData(_target, data->bytes(), data->data(), usage);
  _size = data->bytes();
  _usage = usage;
  set_data(std::move(data));
}

//--------------------------------------------------------------------
void buffer::update_array_gl_buffer(GLuint offset, GLuint size)
{
  auto& data = get_data<std::unique_ptr<array>>();
  if (offset + size > _size)
  {
    buffer_data(std::move(data), _usage);
  }
  else
  {
    buffer_sub_data(offset, size, data->data());
  }
}

//--------------------------------------------------------------------
void buffer::buffer_sub_data(GLint offset, GLsizei size, const GLvoid* data)
{
  bind(_target);
  glBufferSubData(_target, offset, size, data);
}

}
