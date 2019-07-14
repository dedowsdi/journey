#include <iostream>

class Empty
{
};

class Derived : public Empty
{
private:
  int i;
};

int main(int argc, char *argv[])
{
  // c++'s edict doesn't allow zero size freestanding object.
  Empty e;
  std::cout << "size of an Empty object is " << sizeof(e) << std::endl;

  // due to Empty Base Optimization, size of d is an integer.
  Derived d;
  std::cout << "size of Derived object is " << sizeof(d) << std::endl;

  return 0;
}
