#include <iostream>
#include "common.h"

/*
 * T&& is called universal reference,
 * the final t is lvalue reference for lvalue or rvalue reference for rvalue
 *
 * reference collapsing:
 *  & + & = &
 *  && + & = &
 *  & + && = &
 *  && + && = &&
 */
template<typename T>
void f(T&& t)
{
  //std::cout << zxd::type_name<T>() << std::endl;
  std::cout << __PRETTY_FUNCTION__ << std::endl;
}

// note that T& is not const, so you can not pass in non const rref, when you
// pass in const rref, T becomes const sometype, T& becomes const sometype&
// which can bound to anything
template<typename T>
void g(T& t)
{
  //std::cout << zxd::type_name<T>() << std::endl;
  std::cout << __PRETTY_FUNCTION__ << std::endl;
}

int main(int argc, char *argv[])
{
  int i = 0;
  f(5);
  f(i);
  f(static_cast<const int &>(i));
  f(static_cast<int &&>(i)); // exactly the same with std::move
  f(std::move(i));

  // N4606: “...a standard conversion sequence cannot be formed if it requires
  // binding an lvalue reference other than a reference to a non-volatile const
  // type to an rvalue or binding an rvalue reference to an lvalue...”
  
  //g(5);
  g(i);
  g(static_cast<const int &>(i));
  //g(static_cast<int &&>(i)); // you can not bind non const lref to xvalue
  g(static_cast<const int &&>(i)); // this works because T is now const int, T& is const int&
  //g(std::move(i)); // you can not bind non const lref to xvalue

  return 0;
}
