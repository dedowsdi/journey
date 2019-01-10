#ifndef VIRTUAL_FUNCTION_TABLE_VFT_H
#define VIRTUAL_FUNCTION_TABLE_VFT_H

class GameObject
{
public:
  virtual void collide(GameObject& obj);
};

class SpaceShip : public GameObject
{
};

class Asteroid : public GameObject
{
};

#endif /* VIRTUAL_FUNCTION_TABLE_VFT_H */
