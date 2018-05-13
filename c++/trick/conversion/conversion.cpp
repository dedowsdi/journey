#include <iostream>
#include <vector>

template <typename T, typename U>
class Conversion {
  // define two type that's different in size
  typedef char Small;
  class Big {
    char dummy[2];
  };

  // if U can be converted to T, this will be called
  static Small test(U);
  // if U can not be converted to T, this will be called, compiler only choose
  //... when everything else failed, this is the last possible choice.
  static Big test(...);

  // in case T has no public default constructor
  static T MakeT();

public:
  enum { 
    exists = sizeof(test(MakeT())) == sizeof(Small), 
    sameType = false 
  };
};

// specialize to same type
template <typename T>
class Conversion<T, T> {
public:
  enum { exists = 1, sameType = true };
};

#define SUPERSUBCLASS(T, U)                  \
  (Conversion<const U*, const T*>::exists && \
    !Conversion<const T*, const void*>::sameType)

struct A {};

struct B : public A {};

struct C {};

int main() {
  std::cout << Conversion<double, int>::exists << ' '
            << Conversion<char, char*>::exists << ' '
            << Conversion<size_t, std::vector<int>>::exists << std::endl;

  std::cout << SUPERSUBCLASS(A, B) << ' ' << SUPERSUBCLASS(A, C) << std::endl;
}
