#ifndef GL_GLC_MAT_STACK_H
#define GL_GLC_MAT_STACK_H

#include <stack>
#include "gl.h"
#include "glm.h"

namespace zxd
{

using namespace glm;

//mimic gl2 matrix stack
class mat_stack
{
  std::stack<mat4> m_stack;

public:

  mat_stack();
  void push();
  void push_identify();
  void pop();
  const mat4& top() const;
  mat4& top();
  void clear(const mat4& m);

  void rotate(GLfloat r, const vec3& axis);
  void translate(const vec3& v);
  void scale(const vec3& s);

  void load_identity();
  void load_mat4(const mat4& m);

  void multily(const mat4& m);
  void preMultily(const mat4& m);

  GLuint size();
};

}

#endif /* GL_GLC_MAT_STACK_H */
