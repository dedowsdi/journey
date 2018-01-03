#include <iostream>

// this class is devoid of any value, it's very cheap to create an object like
// this. The only purpose of this class is to create an unique type based on
// parameter, which will be used to implement overload.
template <typename T>
struct Type2Type {
  typedef T OriginalType;
};

// general version
template <typename T, typename U>
void foo(const U& arg, Type2Type<T>) {
  std::cout << arg << std::endl;
}

// overload version, make bool special
template <typename U>
void foo(const U& arg, Type2Type<bool>) {
  std::cout << "it a bool : " << arg << std::endl;
}

int main(int argc, char* argv[]) {
  foo(0.15, Type2Type<double>());
  foo(true, Type2Type<bool>());
}
