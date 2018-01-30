#ifndef GEOMETRY_H
#define GEOMETRY_H
#include "glad/glad.h"
#include "glm.h"

namespace zxd {
class geometry {
protected:
  GLuint m_vao;

  GLuint m_vertex_buffer;
  GLuint m_normal_buffer;
  GLuint m_texcoord_buffer;

  vec3_vector m_vertices;
  vec3_vector m_normals;
  vec2_vector m_texcoords;

public:
  geometry() : m_vao(-1) {}

  GLuint vao() const { return m_vao; }

  void bind(GLint vertex, GLint normal = -1, GLint texcoord = -1);
  virtual void draw(GLuint primcount = 1) = 0;

  void build_mesh(GLboolean normal = 1, GLboolean texcoord = 1);
  virtual void build_vertex(){};
  // as i'm using strip and fan, normal should be vertex normal.
  virtual void build_normal(){};
  virtual void build_texcoord(){};

protected:
  void bind_vertex_array_object();
};
}

#endif /* GEOMETRY_H */
