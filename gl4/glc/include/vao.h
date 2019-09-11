#ifndef GL4_GLC_VAO_H
#define GL4_GLC_VAO_H

#include <vector>
#include <memory>
#include <map>

#include <gl.h>

namespace zxd
{
class array;
class arr;
class buffer;

struct vertex_attrib
{
  vertex_attrib() = default;
  vertex_attrib(
    GLuint index_, GLuint size_, GLenum type, std::shared_ptr<buffer> buf_);

  vertex_attrib(GLuint index_, GLuint size_, GLenum type, GLuint stride_,
    GLuint offset_, std::shared_ptr<buffer> buf_);

  GLuint index{0};
  GLuint size{4};
  GLenum type{GL_FLOAT};
  GLboolean normalized{GL_FALSE};
  GLboolean enabled{GL_TRUE};
  GLsizei stride{0};
  GLuint offset{0};
  std::shared_ptr<buffer> buf;
};

class vao
{
public:

  vao();
  ~vao();

  vao(const vao&) = delete;
  vao& operator=(const vao&) = delete;

  // void attrib_pointer(GLuint index, GLuint size, GLfloat type,
    // GLboolean normalized, GLuint stride, void* pointer);

  void attrib_pointer(const vertex_attrib& attrib);

  void bind();

  void enable_attrib_array(GLuint idx);

  void disable_attrib_array(GLuint idx);

  void set_attrib_activity(GLuint idx, bool b);

  // template <typename T>
  // T get_attrib_v(GLuint i, GLenum pname) const;

  GLuint get_object() const { return _object; }

  const vertex_attrib& get_attrib(GLuint index) const;

  using attributes = std::map<GLuint, vertex_attrib>;

private:

  vertex_attrib& get_attrib_pri(GLuint index);

  GLuint _object;
  attributes _attribs;
};

// stride 0 style built in type or glm vertex array.
template <typename T>
void add_vector_attrib(vao& o, GLuint index, const std::vector<T>& vertices,
  GLenum usage = GL_STATIC_DRAW);

void add_array_attrib(vao& o, GLuint index, std::unique_ptr<array> vertices,
  GLenum usage = GL_STATIC_DRAW);

// note that only pointer is stored as buffer_data, no ownership change.
void add_array_attrib(vao& o, GLuint index, const array& vertices,
  GLenum usage = GL_STATIC_DRAW);
}

#endif /* GL4_GLC_VAO_H */
