#ifndef GEOMETRY_H
#define GEOMETRY_H
#include "glad/glad.h"
#include "glm.h"
#include "common.h"

namespace zxd {
class geometry_base {
protected:
  GLuint m_vao;

  GLuint m_vertex_buffer;
  GLuint m_normal_buffer;
  GLuint m_texcoord_buffer;
  GLuint m_tangent_buffer;

public:
  geometry_base() : m_vao(-1) {}
  virtual ~geometry_base(){}

  GLuint vao() const { return m_vao; }

  void bind(
    GLint vertex, GLint normal = -1, GLint texcoord = -1, GLint tangent = -1);
  virtual void draw(GLuint primcount = 1) = 0;

  void build_mesh(
    GLboolean normal = 1, GLboolean texcoord = 1, GLboolean tangent = -1);
  virtual void build_vertex(){};
  // as i'm using strip and fan, normal should be vertex normal.
  virtual void build_normal(){};
  virtual void build_texcoord(){};
  virtual void build_tangent(){};

  // a wrapper of glDrawArrays and glDrawArraysInstanced[ARB]
  void draw_arrays(GLenum mode, GLint first, GLsizei count, GLsizei primcount);

  virtual void buffer_vertex() {}
  virtual void buffer_normal() {}
  virtual void buffer_texcoord() {}
  virtual void buffer_tangent() {}

  virtual void bind_vertex(GLuint index){}
  virtual void bind_normal(GLuint index){}
  virtual void bind_tangent(GLuint index){}
  virtual void bind_texcoord(GLuint index){}

protected:
  void bind_vertex_array_object();
};

template <GLuint s>
struct glm_vec_traits {
  typedef GLfloat vec;
  typedef float_vector vec_vector;
};

template <>
struct glm_vec_traits<2> {
  typedef vec2 vec;
  typedef vec2_vector vec_vector;
};

template <>
struct glm_vec_traits<3> {
  typedef vec3 vec;
  typedef vec3_vector vec_vector;
};

template <>
struct glm_vec_traits<4> {
  typedef vec4 vec;
  typedef vec4_vector vec_vector;
};

template <GLuint s0, GLuint s1, GLuint s2, GLuint s3>
class geometry : public geometry_base {
protected:

  typename glm_vec_traits<s0>::vec_vector m_vertices;
  typename glm_vec_traits<s1>::vec_vector m_normals;
  typename glm_vec_traits<s2>::vec_vector m_texcoords;
  typename glm_vec_traits<s3>::vec_vector m_tangents;

protected:
  virtual void buffer_vertex();
  virtual void buffer_normal();
  virtual void buffer_texcoord();
  virtual void buffer_tangent();

  virtual void bind_vertex(GLuint index);
  virtual void bind_normal(GLuint index);
  virtual void bind_texcoord(GLuint index);
  virtual void bind_tangent(GLuint index);
};

//--------------------------------------------------------------------
template <GLuint s0, GLuint s1, GLuint s2, GLuint s3>
void geometry<s0, s1, s2, s3>::buffer_vertex() {
  glGenBuffers(1, &m_vertex_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
  glBufferData(GL_ARRAY_BUFFER,
    m_vertices.size() * sizeof(decltype(m_vertices[0])),
    value_ptr(m_vertices[0]), GL_STATIC_DRAW);
}

//--------------------------------------------------------------------
template <GLuint s0, GLuint s1, GLuint s2, GLuint s3>
void geometry<s0, s1, s2, s3>::buffer_normal() {
  glGenBuffers(1, &m_normal_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, m_normal_buffer);
  glBufferData(GL_ARRAY_BUFFER,
    m_normals.size() * sizeof(decltype(m_normals[0])), value_ptr(m_normals[0]),
    GL_STATIC_DRAW);
}

//--------------------------------------------------------------------
template <GLuint s0, GLuint s1, GLuint s2, GLuint s3>
void geometry<s0, s1, s2, s3>::buffer_texcoord() {
  glGenBuffers(1, &m_texcoord_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, m_texcoord_buffer);
  glBufferData(GL_ARRAY_BUFFER,
    m_texcoords.size() * sizeof(decltype(m_texcoords[0])),
    value_ptr(m_texcoords[0]), GL_STATIC_DRAW);
}

//--------------------------------------------------------------------
template <GLuint s0, GLuint s1, GLuint s2, GLuint s3>
void geometry<s0, s1, s2, s3>::buffer_tangent() {
  glGenBuffers(1, &m_tangent_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, m_tangent_buffer);
  glBufferData(GL_ARRAY_BUFFER,
    m_tangents.size() * sizeof(decltype(m_tangents[0])),
    value_ptr(m_tangents[0]), GL_STATIC_DRAW);
}

//--------------------------------------------------------------------
template <GLuint s0, GLuint s1, GLuint s2, GLuint s3>
void geometry<s0, s1, s2, s3>::bind_vertex(GLuint index) {
  glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
  glVertexAttribPointer(index, s0, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
  glEnableVertexAttribArray(index);
}

//--------------------------------------------------------------------
template <GLuint s0, GLuint s1, GLuint s2, GLuint s3>
void geometry<s0, s1, s2, s3>::bind_normal(GLuint index) {
  glBindBuffer(GL_ARRAY_BUFFER, m_normal_buffer);
  glVertexAttribPointer(index, s1, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
  glEnableVertexAttribArray(index);
}

//--------------------------------------------------------------------
template <GLuint s0, GLuint s1, GLuint s2, GLuint s3>
void geometry<s0, s1, s2, s3>::bind_texcoord(GLuint index) {
  glBindBuffer(GL_ARRAY_BUFFER, m_texcoord_buffer);
  glVertexAttribPointer(index, s2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
  glEnableVertexAttribArray(index);
}

//--------------------------------------------------------------------
template <GLuint s0, GLuint s1, GLuint s2, GLuint s3>
void geometry<s0, s1, s2, s3>::bind_tangent(GLuint index) {
  glBindBuffer(GL_ARRAY_BUFFER, m_tangent_buffer);
  glVertexAttribPointer(index, s3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
  glEnableVertexAttribArray(index);
}


typedef geometry<3, 3, 2, 3> geometry3323;
typedef geometry<2, 3, 2, 3> geometry2323;
typedef geometry<3, 3, 1, 3> geometry3313;

}

#endif /* GEOMETRY_H */
