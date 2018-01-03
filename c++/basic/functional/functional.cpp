#include <iostream>
#include <functional>
#include <string>

class A {
public:
  void f0() { std::cout << "f0()" << std::endl; }
  void f1(int i) { std::cout << "f0(int i)" << std::endl; }
};

void test_mem_fn() {
  A a;
  auto f0 = std::mem_fn(&A::f0);
  auto f1 = std::mem_fn(&A::f1);
  f0(a);
  f1(a, 1);
}

int main(int argc, char *argv[]) {
  test_mem_fn();

  return 0;
}
