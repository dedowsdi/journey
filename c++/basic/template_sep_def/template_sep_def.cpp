// You can use explicit instantiation or specialization to separate template dec
// and def.

#include <iostream>
#include "a.h"

int main(int argc, char *argv[]) {
  A<int> a0;
  a0.foo();

  foo<int>();
  foo<float>();

  return 0;
}
