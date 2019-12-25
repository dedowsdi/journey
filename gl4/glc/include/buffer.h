#ifndef GL4_GLC_BUFFER_H
#define GL4_GLC_BUFFER_H

#include <memory>
#include <vector>

#include <gl.h>

namespace zxd
{

class array;

class buffer
{
public:

  buffer();
  ~buffer();

  buffer(const buffer& v)= delete;
  buffer& operator=(const buffer& v)= delete;

  void bind(GLenum target);

  // void buffer_data(GLsizei size, const GLvoid* data, GLenum usage);
  void buffer_sub_data(GLint offset, GLsizei size, const GLvoid* data);

  void buffer_data(std::shared_ptr<array> data, GLenum usage);

  // update gl buffer to underlying array buffer
  void update_array_gl_buffer(GLuint offset, GLuint size);

  GLuint get_object() const { return _object; }

  std::shared_ptr<array> get_data() const { return _data; }

  void set_data(std::shared_ptr<array> data)
  {
    _data = data;
  }

private:
  GLenum _target{GL_ARRAY_BUFFER};
  GLenum _usage{GL_STATIC_DRAW};
  GLuint _object{0};
  GLuint _size{0};
  std::shared_ptr<array> _data;
};

}

#endif /* GL4_GLC_BUFFER_H */
