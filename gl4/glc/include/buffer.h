#ifndef GL4_GLC_BUFFER_H
#define GL4_GLC_BUFFER_H

#include <memory>
#include <vector>

#include <gl.h>

namespace zxd
{

class array;

class buffer_data_base
{
public:
  template <typename T>
  const T& get() const;

  template <typename T>
  T& get();
};

template <typename T>
class buffer_data : public buffer_data_base
{
public:
  buffer_data(const T& data);
  buffer_data(T&& data);

  const T& get_data() const { return _data; }
  T& get_data() { return _data; }

  void set_data(const T& v){ _data = v; }
  void set_data(T&& v){ _data = std::move(v); }

private:
  T _data;
};

//--------------------------------------------------------------------
template <typename T>
const T& buffer_data_base::get() const
{
  return static_cast<buffer_data<T>*>(this)->get_data();
}

//--------------------------------------------------------------------
template <typename T>
T& buffer_data_base::get()
{
  return static_cast<buffer_data<T>*>(this)->get_data();
}

//--------------------------------------------------------------------
template<typename T>
buffer_data<T>::buffer_data(const T& data):
  _data(data)
{
}

//--------------------------------------------------------------------
template<typename T>
buffer_data<T>::buffer_data(T&& data):
  _data(std::move(data))
{
}

class buffer
{
public:

  buffer();
  ~buffer();

  buffer(const buffer& v)= delete;
  buffer& operator=(const buffer& v)= delete;

  void bind(GLenum target);

  void buffer_data(GLsizei size, const GLvoid* data, GLenum usage);
  void buffer_sub_data(GLint offset, GLsizei size, const GLvoid* data);

  template <typename T>
  void buffer_data(const std::vector<T>& data, GLenum usage)
  {
    buffer_data(data.size() * sizeof(data.front()), &data.front(), usage);
    set_data(data);
  }

  template <typename T>
  void buffer_data(std::vector<T>&& data, GLenum usage)
  {
    buffer_data(data.size() * sizeof(data.front()), &data.front(), usage);
    set_data(std::move(data));
  }

  void buffer_data(std::unique_ptr<array> data, GLenum usage);

  // update gl buffer to underlying array buffer
  void update_array_gl_buffer(GLuint offset, GLuint size);

  GLuint get_object() const { return _object; }

  template <typename T>
  const T& get_data() const { return _data->get<T>(); }

  template <typename T>
  T& get_data() { return _data->get<T>(); }

  template <typename T>
  void set_data(T&& data)
  {
    _data = std::make_unique<zxd::buffer_data<std::decay_t<T>>>(
      std::forward<T>(data));
  }

private:
  GLenum _target{GL_ARRAY_BUFFER};
  GLenum _usage{GL_STATIC_DRAW};
  GLuint _object{0};
  GLuint _size{0};
  std::unique_ptr<buffer_data_base> _data;
};

}

#endif /* GL4_GLC_BUFFER_H */
