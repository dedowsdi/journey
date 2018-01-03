#include <iostream>

// Int2Type generates a distinct type for each distinct constant integral value
// passed.
template <int v>
struct Int2Type {
  enum { value = v };
};

template <int i>
void fun() {
  std::cout << i << std::endl;
}

template <>
void fun<0>() {
  std::cout << "\"" << 0 << "\"" << std::endl;
}

class A {
public:
  void fun() const { std::cout << "A:fun()" << std::endl; }
};

template <typename T, bool isA>
void haveFun(const T& t) {
  // compiler will tried to compile both branch, this will always fail, need to
  // find a way to tell compiler compile only 1 "branch"
  // if (isA) {
  // t.fun();   // error if t is not A
  //} else {
  // std::cout << t << std::endl;  // error if t is A
  //}

  // compiler doesn't not compile template function that's never used, it only
  // checks their syntax
  haveFun(t, Int2Type<isA>());
}

template <typename T>
void haveFun(const T& t, Int2Type<1> isA) {
  t.fun();
}

template <typename T>
void haveFun(const T& t, Int2Type<0> isA) {
  std::cout << t << std::endl;
}

int main(int argc, char* argv[]) {
  fun<-1>();
  fun<0>();
  fun<1>();

  A a;
  haveFun<A, true>(a);
  haveFun<int, false>(5);

  return 0;
}
