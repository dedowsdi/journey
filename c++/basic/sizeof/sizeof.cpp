/*
 * The only expression that fail sizeof is function pointer and default
 * constructor.
 *
 * To make it work with default constructor:
 *    add parenthesis if the expression no template parameter
 *    otherwise use tempalte<typename T> makeType
 *
 * sizeof never truely evaluate the expression.
 */
#include <iostream>

#define printSize(type) \
  std::cout << "size of " #type " is " << sizeof(type) << std::endl;

// struct takes at least 1 byte even it's empty
struct A {};

struct B {
  B() {}
  B(int i) {}
  bool i;
};

template <bool>
struct C {
  C(...) {}
};


struct D {
  bool b;
  float f;
  float f1[2];
};

template <typename T>
T makeType();

int foo() { return 5; }

int main(int argc, char *argv[]) {
  printSize(bool);
  printSize(char);
  printSize(short);
  printSize(int);
  printSize(long);
  printSize(float);
  printSize(double);
  printSize(void *);
  printSize(A);
  printSize(B);
  printSize(D);

  B b;
  printSize(b);
  // printSize(B());  // can't use sizeof to a function type ?
  printSize((B()));  // this is ok as long as B has no template parameter
  printSize(B(5));   // B() doesn't work, but B(5) works!
  printSize(C<true>);
  printSize(C<false>(5));
  printSize(foo());
  // printSize(C<true>((A()))); // failed even for (A())
  printSize(C<true>(makeType<A>()));  // thanks to makeType

  return 0;
}
