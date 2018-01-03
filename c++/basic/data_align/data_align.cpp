/*
 * Declare primitive properties in sequence of sizeof datatype.
 */
#include <iostream>

#define printSize(type) \
  std::cout << "size of " #type " is " << sizeof(type) << std::endl;

// properties will be aligned to max(8, max sizeof(property))
struct A {
  bool i;  // aligne to short
  short j;
};

struct B {
  bool i;  // aligne to int
  int j;
};

struct C {
  bool i;           // 8
  int j;            // 4
  bool k0, k1, k2;  // 4
  double k;         // 8
};

struct D {
  A a;
  double b;
};

struct E {
  bool b;  // 8
  C c;     // 24
};

int main(int argc, char *argv[]) {
  printSize(A);
  printSize(B);
  printSize(C);
  printSize(D);
  printSize(E);
  return 0;
}
