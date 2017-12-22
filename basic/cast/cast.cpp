#include <iostream>

enum Monster { MONSTER_DOG = 0 };

void foo() { std::cout << "foo()" << std::endl; }
typedef void (*FuncPtr)();

class A {
public:
  A(){};
  A(int i) { std::cout << "implicit convert int to A" << std::endl; }
};

class B : public A {};

void staticCast() {
  B b;
  A* a = &b;

  A a1 = static_cast<A>(5);
  static_cast<void>(a1);
  std::cout << "1.use static_cast<new_type>(expr) to implicit convert"
            << std::endl;

  B* b1 = static_cast<B*>(a);
  std::cout << "2.use static_cast<Derived*>(Base*) to downcast" << std::endl;

  static_cast<void>(b1);
  std::cout << "4.use static_cast<void>(exp) to discard unused variable"
            << std::endl;

  B& b2 = b;
  B&& b3 = static_cast<B>(b2);  // very weird
  static_cast<void>(b3);

  std::cout << "6.use static_cast<new_type>(exp) to cast lvalue-to-rvalue, "
               "[optional]array-to-pointer, or [optional]function-to-pointer"
            << std::endl;

  Monster m = static_cast<Monster>(0);
  static_cast<void>(m);
  std::cout << "8.use static_cast<Enum>(exp) to cast integer type to enum"
            << std::endl;

  a = static_cast<A*>(b1);
  std::cout << "9.[optional] use static_cast<Base*>(Derived*) to upcast"
            << std::endl;
}

int main(int argc, char* argv[]) {
  staticCast();
  int i = 1;
  short s = 2;
  char c = 3;
  i = s;
  s = c;
  c = s;
  c = i;

  return 0;
}
