#include <iostream>
#include <vector>

template <typename Container>
typename Container::iterator begin_iter(Container& c)
{
  // Container::iterator is a dependent name. By default, c++ assume that
  // any name whose lookup is dependent on a template parameter refers to a
  // non-type, non-template, plain old variable/function/object_style entity.
  typename Container::iterator iter = c.begin();
  return iter;
}

struct s0
{
  template<int N> static void A(int){};
  template<int N> struct B{};
};

template <typename T>
void foo()
{
  // we must tell compiler that A is a template function
  T::template A<0>(5);
}

template <typename T>
void goo()
{
  // we must tell compiler that B is a template type
  typename T::template B<5> x;
}

int main(int argc, char *argv[])
{

  std::vector<int> v;
  auto b = begin_iter(v);

  foo<s0>();
  goo<s0>();
  
  return 0;
}
