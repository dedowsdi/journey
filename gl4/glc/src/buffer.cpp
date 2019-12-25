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
  }
}

//--------------------------------------------------------------------
void buffer::bind(GLenum target)
{
  glBindBuffer(target, _object);
  _target = target;
}

//--------------------------------------------------------------------
void buffer::buffer_data(std::shared_ptr<array> data, GLenum usage)
{
  glBufferData(_target, data->bytes(), data->data(), usage);
  _size = data->bytes();
  _usage = usage;
  set_data(data);
}

//--------------------------------------------------------------------
void buffer::update_array_gl_buffer(GLuint offset, GLuint size)
{
  auto data = get_data();
  if (offset + size > _size)
  {
    buffer_data(data, _usage);
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
