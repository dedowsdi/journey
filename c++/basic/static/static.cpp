#include <iostream>

class A {
public:
  A() { std::cout << "A()" << std::endl; }
  static int i;
};

int A::i = 5;

void foo() {
  std::cout << "foo() begin" << std::endl;
  // function local static will be initialized at the first time it's called.
  static A a;
  std::cout << "foo() end" << std::endl;
}

int main(int argc, char *argv[]) {
  std::cout << "main() begin" << std::endl;
  foo();
  foo();
  std::cout << "main() end" << std::endl;

  return 0;
}
