#ifndef VIRTUAL_FUNCTION_TABLE_COLLISION_MANAGER_H
#define VIRTUAL_FUNCTION_TABLE_COLLISION_MANAGER_H

#include <functional>
#include <typeindex>
#include <map>

class GameObject;

class CollisionManager
{
public:
  using hit_func = std::function<void(GameObject&, GameObject&)>;
  void addEntry(const std::type_index& type0, const std::type_index& type1, hit_func func);
  void removeEntry(const std::type_index& type0, const std::type_index& type1);
  hit_func lookup(const std::type_index& type0, const std::type_index& type1);

  static CollisionManager& instance();

  CollisionManager(const CollisionManager& v) = delete;
  CollisionManager& operator=(const CollisionManager& v) = delete;

private:
  CollisionManager(){}

  std::map<std::pair<std::type_index,std::type_index>, hit_func> m_entries;
};


#endif /* VIRTUAL_FUNCTION_TABLE_COLLISION_MANAGER_H */
