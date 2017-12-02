/*
 * There has no partial specialization for template function or method, you have
 * to use overload to do this kind of job.
 */
#include <iostream>

template <typename T0, typename T1>
void foo() {
  std::cout << "<T0, T1>foo called" << std::endl;
}

template <typename T0>
void foo() {
  std::cout << "<T>foo called" << std::endl;
}

int main(int argc, char *argv[]) {
  foo<int, int>();
  foo<int>();

  return 0;
}
