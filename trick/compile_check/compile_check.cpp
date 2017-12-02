/*
 * just a demostration, you should use std::static_assert if you use c++11
 */
#include <iostream>

template <typename T>
T makeType();

template <bool>
struct CompileTimeChecker {
  CompileTimeChecker(...);
};

// specialize false that takes no ellipsis
template <>
struct CompileTimeChecker<false> {};

#define STATIC_CHECK(expr, msg)                                             \
  {                                                                         \
    class ERROR_##msg {};                                                   \
    ERROR_##msg e;                                                          \
    (void)sizeof(CompileTimeChecker<(expr) != 0>(makeType<ERROR_##msg>())); \
  }

template <class To, class From>
To safe_reinterpret_cast(From from) {
  STATIC_CHECK(sizeof(From) <= sizeof(To), Destination_Type_Too_Narrow);
  return reinterpret_cast<To>(from);
}

int main(int argc, char *argv[]) {
  int i = 5;
  void *ip = &i;
  char d = safe_reinterpret_cast<char>(ip);
  (void)d;

  return 0;
}
