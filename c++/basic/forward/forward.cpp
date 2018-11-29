#include <iostream>

class A
{
};

A getA()
{
  return A();
}

// t is called forwarding reference(a function argument that is an rvalue
// reference to a cv-unqualified function template parameter)
template <typename T>
void f0(T&& t)
{
  //  Even if the variable's type is rvalue reference, the expression consisting
  //  of its name is an lvalue expression
  g(t); 
}

template <typename T>
void f1(T&& t)
{
  g(std::move(t)); // result in always rvalue arg
}

template <typename T>
void f2(T&& t)
{
  g(std::forward<T>(t)); // perfect forward
}

void g(const A& a)
{
  std::cout << "g(const A&)" << std::endl;
}

void g(A&& a)
{
  std::cout << "g(A&&)" << std::endl;
}

int main(int argc, char *argv[])
{
  A a;
  f0(getA());
  f0(a);

  f1(getA());
  f1(a);

  f2(getA());
  f2(a);
  
  return 0;
}
