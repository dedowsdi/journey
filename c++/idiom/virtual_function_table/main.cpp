#include "vft.h"
#include "collision_manager.h"
#include <iostream>
#include <typeindex>

// actural test function is outside the class of hierachy, so you don't have to
// change interface everytime you add some new class the the hierachy.
//
// you have to use base object type, function pointer match ignores polymorphism
void shipHitAsteroid(GameObject& ship, GameObject& asteroid)
{
  // rrti is required
  auto& s = dynamic_cast<SpaceShip&>(ship);
  auto& a = dynamic_cast<Asteroid&>(asteroid);
  (void)s;
  (void)a;
  std::cout << __PRETTY_FUNCTION__ << std::endl;
}

void shipHitShip(GameObject& ship0, GameObject& ship1)
{
  std::cout << __PRETTY_FUNCTION__ << std::endl;
}

void asteroidHitAsteroid(GameObject& a0, GameObject& a1)
{
  std::cout << __PRETTY_FUNCTION__ << std::endl;
}

int main(int argc, char *argv[])
{
  auto& instance = CollisionManager::instance();
  instance.addEntry(std::type_index(typeid(SpaceShip)), std::type_index(typeid(SpaceShip)), shipHitShip);
  instance.addEntry(std::type_index(typeid(SpaceShip)), std::type_index(typeid(Asteroid)), shipHitAsteroid);
  instance.addEntry(std::type_index(typeid(Asteroid)), std::type_index(typeid(Asteroid)), asteroidHitAsteroid);

  SpaceShip obj0;
  Asteroid  obj1;
  obj0.collide(obj0);
  obj0.collide(obj1);
  obj1.collide(obj0);
  obj1.collide(obj1);
  
  return 0;
}
