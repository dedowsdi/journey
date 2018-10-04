#ifndef MATSTACK_H
#define MAT_STACK_H

#include "glm.h"
#include <stack>

namespace zxd
{

//mimic gl2 matrix stack
class MatStack
{
  std::stack<osg::Matrix> m_stack;

public:

  MatStack();
  void push();
  void push_identify();
  void pop();
  const osg::Matrix& top() const;
  osg::Matrix& top();

  void rotate(float r, const osg::Vec3& axis);
  void translate(const osg::Vec3& v);
  void scale(const osg::Vec3& s);

  void load_identity();
  void load_mat4(const osg::Matrix& m);

  void postMultily(const osg::Matrix& m);
  void preMultily(const osg::Matrix& m);

  unsigned size();
};

}


#endif /* MATSTACK_H */
