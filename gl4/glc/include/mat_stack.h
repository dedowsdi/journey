#ifndef MAT_STACK_H
#define MAT_STACK_H

#include "glm.h"
#include <stack>

namespace zxd
{

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


#endif /* MAT_STACK_H */
