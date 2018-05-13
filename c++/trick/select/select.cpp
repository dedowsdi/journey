#include <iostream>

// select T or U based on flag
template <bool flag, typename T, typename U>
struct Select {
  typedef T Result;
};

template <typename T, typename U>
struct Select<false, T, U> {
  typedef U Result;
};

class A {
public:
  void fun() { std::cout << "A::fun()" << std::endl; }
};

class B {
public:
  void fun() { std::cout << "B::fun()" << std::endl; }
};

int main(int argc, char *argv[]) {
  Select<true, A, B>::Result obj0;
  obj0.fun();
  Select<false, A, B>::Result obj1;
  obj1.fun();

  return 0;
}
