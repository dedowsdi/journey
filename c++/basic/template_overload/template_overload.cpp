/*
 * There has no partial specialization for template function or method, you have
 * to use overload to do this kind of job.
 */
#include <iostream>

template <typename T0, typename T1>
void foo() {
  std::cout << __PRETTY_FUNCTION__ << std::endl;
}

template <typename T0>
void foo() {
  std::cout << __PRETTY_FUNCTION__ << std::endl;
}

int main(int argc, char *argv[]) {
  foo<int, int>();
  foo<int>();

  return 0;
}
