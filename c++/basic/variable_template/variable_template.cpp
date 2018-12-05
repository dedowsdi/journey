/*
 * c++ 14 feature
 */
#include <iostream>

// pi by type
template <typename T>
constexpr T pi = T(3.1415926535897932385L);

template<class T>
T circular_area(T r) // function template
{
    return pi<T> * r * r; // pi<T> is a variable template instantiation
}

struct size
{
  // with variable template, you can replace a bunch of static .... with one
  template <typename T>
  static int value; // 100% of the time, it's static inside a class
};

// note the position of <int>
template <>
int size::value<int> = 4;

template <>
int size::value<short> = 2;

int main(int argc, char *argv[])
{
  std::cout << size::value<short> << std::endl;
  
  return 0;
}
