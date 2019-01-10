#include "vft.h"
#include "collision_manager.h"
#include <typeindex>
#include <typeinfo>

//--------------------------------------------------------------------
void GameObject::collide(GameObject& obj)
{
  auto& cm = CollisionManager::instance();
  auto test = cm.lookup(std::type_index(typeid(*this)), std::type_index(typeid(obj)));
  test(*this, obj);
}
