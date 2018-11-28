/*
 * lifetime of temporary object will be extened by the first const reference
 * bound to it, it's destoryed after the life time of that const reference
 * expired
 */
#include <iostream>
#include <algorithm>

class A
{
public:
  A() = default;
  ~A()
  {
    std::cout << "~A()" << std::endl;
  }
};

bool operator< (const A& lhs, const A& rhs)
{
  return rand()%2 == 1;
}

// return value
A getA()
{
  return A();
}


int main(int argc, char *argv[])
{
  // std::min use const reference arg, so lifetime of temporary object returned
  // by getA() will be extended by arg of std::min, it will be destroyed after
  // std::min finished, so following statement will cause undefined behavor, be
  // very very careful with.
  const A& a = std::min(getA(), getA());
  // do what every with a
  std::cout << "2" << std::endl;
  
  return 0;
}
