#include "movable.h"

namespace zxd
{

//--------------------------------------------------------------------
void movable::update()
{
  m_vel += m_acc;
  m_pos += m_vel;
  m_acc = vec3(0);
}

//--------------------------------------------------------------------
void movable::apply_force(const vec3& force)
{
  m_acc += force;
}
}
