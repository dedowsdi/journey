#include "movable.h"

namespace zxd
{

//--------------------------------------------------------------------
void movable::udpate()
{
  m_pos += m_vel;
  m_vel += m_acc;
  m_acc = vec3(0);
}

//--------------------------------------------------------------------
void movable::applyForce(const vec3& force)
{
  m_acc += force;
}
}
