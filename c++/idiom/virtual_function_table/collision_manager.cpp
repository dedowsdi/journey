#include "collision_manager.h"
#include <algorithm>
#include <functional>

//--------------------------------------------------------------------
void CollisionManager::addEntry(const std::type_index& type0, const std::type_index& type1, hit_func func)
{
  m_entries.insert(std::make_pair(std::make_pair(type0, type1), func));
  m_entries.insert(std::make_pair(std::make_pair(type1, type0),
        std::bind(func, std::placeholders::_2, std::placeholders::_1)
        ));
}

//--------------------------------------------------------------------
void CollisionManager::removeEntry(const std::type_index& type0, const std::type_index& type1)
{
  auto iter = m_entries.find(std::make_pair(type0, type1));
  if(iter != m_entries.end())
    m_entries.erase(iter);
  iter = m_entries.find(std::make_pair(type1, type0));
  if(iter != m_entries.end())
    m_entries.erase(iter);
}

//--------------------------------------------------------------------
CollisionManager::hit_func CollisionManager::lookup(const std::type_index& type0,
    const std::type_index& type1)
{
  auto iter = m_entries.find(std::make_pair(type0, type1));
  return iter == m_entries.end() ? nullptr : iter->second;
}

//--------------------------------------------------------------------
CollisionManager& CollisionManager::instance()
{
  static CollisionManager cm;
  return cm;
}
