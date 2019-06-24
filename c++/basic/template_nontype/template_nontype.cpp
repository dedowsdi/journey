/*
 * Nontype parameters are replaced by value when the function is called;
 * You can only feed literal or const variabe to nontype parameter
 */
#include <iostream>

template <int T>
void foo() {
  std::cout << T << std::endl;
}

template <class T, size_t N>
void array_init(T (&param)[N]) {
  for (size_t i = 0; i < N; ++i) {
    param[i] = 0;
  }
}

int main(int argc, char *argv[]) {
  const int i = 1;
  foo<0>();
  foo<i>();

  // int j = 2;
  // foo<j>(); // j is not constant

  int ia[10];
  array_init(ia); //instantiate array_init(int(&)[42])

  return 0;
}
