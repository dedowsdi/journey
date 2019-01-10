#ifndef DOUBLE_DISPATCH_DOUBLE_DISPATH_H
#define DOUBLE_DISPATCH_DOUBLE_DISPATH_H

#include <iostream>

// change from more effective c++ item 31
//
// drawback of this approach is that every time you add a new class in the
// hierachy, you need to change interface of all the objects in the hierachy,
// and you need to recompile it.

class SpaceShip;
class Asteroid;

class GameObject
{
public:
  virtual void collide(GameObject& obj) = 0;
};

class SpaceShip : public GameObject
{
public:
  // first dispatch
  void collide(GameObject& obj) override
  {
    // compiler will choose proper function based on static type of *this
    obj.collide(*this);
  }

  // second dispatch
  void collide(SpaceShip& obj)
  {
    std::cout << __PRETTY_FUNCTION__ << std::endl;
  }

  void collide(Asteroid& obj)
  {
    std::cout << __PRETTY_FUNCTION__ << std::endl;
  }
};

class Asteroid : public GameObject
{
public:
  void collide(GameObject& obj) override
  {
    obj.collide(*this);
  }

  void collide(SpaceShip& obj)
  {
    std::cout << __PRETTY_FUNCTION__ << std::endl;
  }

  void collide(Asteroid& obj)
  {
    std::cout << __PRETTY_FUNCTION__ << std::endl;
  }
};

#endif /* DOUBLE_DISPATCH_DOUBLE_DISPATH_H */
