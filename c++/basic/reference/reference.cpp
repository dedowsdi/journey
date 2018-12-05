#include <iostream>

class A
{
public:
  A()
  {
    // MSVC use __FUCSIG__
    std::cout << __PRETTY_FUNCTION__ << std::endl;
  }

  A(const A& a)
  {
    std::cout << __PRETTY_FUNCTION__ << std::endl;
  }
};

class B
{
public:
  B(const A& a)
  {
    std::cout << __PRETTY_FUNCTION__ << std::endl;
  }
};

int main(int argc, char *argv[])
{
  A a;

  // you can not bind lvalue to rvalue reference
  //A&& t = a;
  
  // here t is bound to a temporary object of B, not A, be very very careful
  // about this.
  B&& t = a;
  
  return 0;
}
