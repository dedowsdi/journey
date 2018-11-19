#ifndef MOVABLE_H
#define MOVABLE_H
#include "glm.h"

namespace zxd
{

class movable
{
protected:
  vec3 m_pos = vec3(0);
  vec3 m_vel = vec3(0);
  vec3 m_acc = vec3(0);

public:

  const vec3& pos() const { return m_pos; }
  void pos(const vec3& v){ m_pos = v; }

  const vec3& vel() const { return m_vel; }
  void vel(const vec3& v){ m_vel = v; }

  const vec3& acc() const { return m_acc; }
  void acc(const vec3& v){ m_acc = v; }

  virtual void update();
  void apply_force(const vec3& force);
};

}


#endif /* MOVABLE_H */
