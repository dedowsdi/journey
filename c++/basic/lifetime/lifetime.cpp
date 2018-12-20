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
    std::cout << __PRETTY_FUNCTION__ << std::endl;
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

int foo(const A& a)
{
  std::cout << __PRETTY_FUNCTION__ << std::endl;
  return 5;
}

int goo(int i)
{
  std::cout << __PRETTY_FUNCTION__ << std::endl;
  return i;
}

void hoo(int i)
{
  std::cout << __PRETTY_FUNCTION__ << std::endl;
}


int main(int argc, char *argv[])
{
  // std::min use const reference arg, so lifetime of temporary object returned
  // by getA() will be extended by arg of std::min, it will be destroyed after
  // std::min finished, so following statement will cause undefined behavor, be
  // very very careful with.
  const A& a = std::min(getA(), getA());
  std::cout << "0" << std::endl;
  {
    A b;
    {
      // if you bind const lref to rref, no temporary object exists, becareful
      const A& a = std::move(b);
      std::cout << "1" << std::endl;
      // life time of a ends here
    }
    std::cout << "2" << std::endl;
  }

  std::cout << "------------------" << std::endl;

  {
    getA(); // destroyed immediately
    A&& c = getA(); // life time of return value will be bound to c
    std::cout << "3" << std::endl;
  }

  std::cout << "------------------" << std::endl;
  {
    hoo(goo(foo(getA()))); // A will be destroyed after this statement
    std::cout << "4" << std::endl;
  }

  std::cout << "------------------" << std::endl;
  // do what every with a
  std::cout << "9" << std::endl;
  
  return 0;
}
