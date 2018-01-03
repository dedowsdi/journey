#include <iostream>

void foo() { std::cout << "foo()" << std::endl; }

void foo(int i) { std::cout << "foo(int)" << std::endl; }

class A {
public:
  void foo() { std::cout << "A::foo()" << std::endl; }
};

int main(int argc, char* argv[]) {
  void (*p)() = foo;  // choose foo() automatically
  p();                // invoked through pointer
  (*p)();             // invoked through lvalue reference

  void (&r0)(int) = foo;  // function lvalue referece
  r0(5);                  // invoke through lvalue reference

  void (&r1)() = *p;
  r1();

  void (A::*m0)() = &A::foo;  // method pointer
  A a;
  (a.*m0)();  // need an object to invoke methed pointer
  A* pa = &a;
  (pa->*m0)();

  // there has no method reference
  // void (A::&m0)() = &A::foo;  // method pointer

  return 0;
}
