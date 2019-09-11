#include <vao.h>

#include <utility>
#include <iostream>

#include <exception.h>
#include <buffer.h>
#include <glm_traits.h>
#include <array.h>

#ifndef BUFFER_OFFSET
#define BUFFER_OFFSET(bytes) ((GLubyte*)NULL + (bytes))
#endif

namespace zxd
{

//--------------------------------------------------------------------
vertex_attrib::vertex_attrib(
  GLuint index_, GLuint size_, GLenum type, std::shared_ptr<buffer> buf_)
    : vertex_attrib(index_, size_, type, 0, 0, buf_)
{
}

//--------------------------------------------------------------------
vertex_attrib::vertex_attrib(GLuint index_, GLuint size_, GLenum type,
  GLuint stride_, GLuint offset_, std::shared_ptr<buffer> buf_)
    : index(index_), size(size_), stride(stride_), offset(offset_), buf(buf_)
{
}

//--------------------------------------------------------------------
vao::vao()
{
  glGenVertexArrays(1, &_object);
}

//--------------------------------------------------------------------
vao::~vao()
{
  if(_object != 0)
  {
    glDeleteVertexArrays(1, &_object);
    std::cout << "vao " << _object << " deleted"  << std::endl;
  }
}

////--------------------------------------------------------------------
//void vao::attrib_pointer(GLuint index, GLuint size, GLfloat type,
//  GLboolean normalized, GLuint stride, void* pointer)
//{
//  vertex_attrib attrib(
//    index, size, stride, normalized, std::shared_ptr<buffer>());
//  attrib.set_type(type);
//}

//--------------------------------------------------------------------
void vao::attrib_pointer(const vertex_attrib& attrib)
{
  _attribs[attrib.index] = attrib;
  glVertexAttribPointer(attrib.index, attrib.size, attrib.type,
    attrib.normalized, attrib.stride, BUFFER_OFFSET(attrib.offset));
  if (attrib.enabled)
    glEnableVertexAttribArray(attrib.index);
  else
    glDisableVertexAttribArray(attrib.index);
}

//--------------------------------------------------------------------
void vao::bind()
{
  glBindVertexArray(_object);
}

//--------------------------------------------------------------------
void vao::enable_attrib_array(GLuint idx)
{
  auto& attrib = get_attrib(idx);
  attrib.enabled = GL_TRUE;
  glEnableVertexAttribArray(idx);
}

//--------------------------------------------------------------------
void vao::disable_attrib_array(GLuint idx)
{
  auto& attrib = get_attrib(idx);
  attrib.enabled = GL_FALSE;
  glDisableVertexAttribArray(idx);
}

//--------------------------------------------------------------------
void vao::set_attrib_activity(GLuint idx, bool b)
{
  if (b)
  {
    enable_attrib_array(idx);
  }
  else
  {
    disable_attrib_array(idx);
  }
}

//--------------------------------------------------------------------
// template <typename T>
// T vao::get_attrib_v(GLuint i, GLenum pname) const
// {

// }

//--------------------------------------------------------------------
const vertex_attrib& vao::get_attrib(GLuint index) const
{
  auto iter = _attribs.find(index);
  if (iter == _attribs.end())
  {
    throw gl_not_found("invalid attrib index" + std::to_string(index));
  }
  return iter->second;
}

//--------------------------------------------------------------------
vertex_attrib& vao::get_attrib(GLuint index)
{
  auto iter = _attribs.find(index);
  if (iter == _attribs.end())
  {
    throw gl_not_found("invalid attrib index" + std::to_string(index));
  }
  return iter->second;
}

//--------------------------------------------------------------------
template <typename T>
void add_vector_attrib(
  vao& o, GLuint index, const std::vector<T>& vertices, GLenum usage)
{
  o.bind();
  auto buf = std::make_shared<buffer>();
  buf->bind(GL_ARRAY_BUFFER);
  buf->buffer_data(vertices, usage);

  vertex_attrib attrib(index, glm_type_components<T>, glm_type_enum<T>, buf);
  o.attrib_pointer(attrib);
}

//--------------------------------------------------------------------
void add_array_attrib(
  vao& o, GLuint index, std::unique_ptr<array> vertices, GLenum usage)
{
  o.bind();
  auto buf = std::make_shared<buffer>();
  buf->bind(GL_ARRAY_BUFFER);

  vertex_attrib attrib(index, vertices->element_size(), vertices->gltype(), buf);
  o.attrib_pointer(attrib);

  buf->buffer_data(vertices->bytes(), vertices->data(), usage);
  buf->set_data<std::unique_ptr<array>>(std::move(vertices));
}

//--------------------------------------------------------------------
void add_array_attrib(vao& o, GLuint index, const array& vertices,
  GLenum usage)
{
  o.bind();
  auto buf = std::make_shared<buffer>();
  buf->bind(GL_ARRAY_BUFFER);

  vertex_attrib attrib(index, vertices.element_size(), vertices.gltype(), buf);
  o.attrib_pointer(attrib);

  buf->buffer_data(vertices.bytes(), vertices.data(), usage);
  buf->set_data<const array*>(&vertices);
}

}
