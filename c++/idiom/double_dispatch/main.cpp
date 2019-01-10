#include "double_dispatch.h"

int main(int argc, char *argv[])
{
  SpaceShip obj0;
  Asteroid  obj1;
  obj0.collide(obj0);
  obj0.collide(obj1);
  obj1.collide(obj0);
  obj1.collide(obj1);
  return 0;
}
