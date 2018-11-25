/*
 * first of first, 
 * T t = ... is totally the same as T t(...)
 *
 * if you define list initialization, {} will always pick list initialization
 * version constructor, otherwise it will degraded to normal one.
 */
#include <iostream>
#include <string>

class Monster
{
public:
  int i;

  Monster(int i_):
    i(i_)
  {
    std::cout << "Monster(int i_)" << std::endl;
  }

  Monster(const std::string& s)
  {
    std::cout << "Monster(const std::string& s)" << std::endl;
  }

  Monster(const Monster& m)
  {
    i = m.i;
    std::cout << "Monster(const Monster& m)" << std::endl;
  }

  // if you don't list initialization, Monster m1 = {5} will degrad to Monster m1 = 5;
  Monster(const std::initializer_list<int>& il)
  {
    i = *il.begin();
    std::cout << "Monster(std::initializer_list<int>& il)" << std::endl;
  }

};

int main(int argc, char *argv[])
{
  Monster m0 = 5;
  Monster m01 = Monster(5); // the same as above, careful, just ignore the =
  Monster m02 = Monster(m0); // copy constructor, careful, just ignore the =
  Monster m03 = m0; // normal copy constructor
  Monster m04{5}; // list initialization!
  Monster m05{std::string("abc")}; // list initialization!
  std::cout << "-------------------------------" << std::endl;
  Monster m1 = {5};
  Monster m11 = Monster{5};
  Monster m12 = Monster{m1}; // copy constructor, careful, just ignore the =
  Monster m2 = {5, 2};
  Monster m3 = std::string("abc");
  
  return 0;
}

