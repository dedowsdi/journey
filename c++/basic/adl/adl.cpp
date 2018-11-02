/*
 * argument dependent lookup, also know as koenig lookup.
 */
#include <iostream>

namespace zxd
{
  struct A
  {
  };
  void test(const A& a)
  {

  }
}

class B
{
  friend void swap(B& b0, B& b1)
  {
    std::cout << "B::swap(..)"<< std::endl;
  }
};


int main(int argc, char *argv[])
{
  zxd::A a;

  // don't need zxd::
  test(a);

  B b0, b1;

  using std::swap;
  swap(b0, b1);
  
  return 0;
}
