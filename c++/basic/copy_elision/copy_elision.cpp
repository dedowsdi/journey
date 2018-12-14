/*
 * must be tested in release mode
 * copy elision or named return value elision is implemented by most compiler
 *
 * the calling function allocates space for the return value on its stack, and
 * passes the address of that memory to the callee. The callee can then
 * construct a return value directly into that space, which eliminates the need
 * to copy from inside to outside. The copy is simply elided, or “edited out,”
 * by the compiler.
 *
 * if you return parameter, the calling will alwalys request a copy, there is
 * also a copy if you pass parameter by lvalue.
 */
#include <iostream>


class A
{
public:
  A()
  {
    std::cout << __PRETTY_FUNCTION__ << std::endl;
  }

  A(const A& a)
  {
    std::cout << __PRETTY_FUNCTION__ << std::endl;
  }

  A& operator=(const A& a)
  {
    std::cout << __PRETTY_FUNCTION__ << std::endl;
    return *this;
  }

};

// no copy in return
A generate_a()
{
  std::cout << "generate_a()" << std::endl;
  return A();
}

// still no copy in return, but one copy exists during parameter init if you
// passin l value
A return_param(A a)
{
  std::cout << __PRETTY_FUNCTION__ << std::endl;
  return a;
}

A return_int_param(int i)
{
  std::cout << __PRETTY_FUNCTION__ << std::endl;
  return A();
}

int main(int argc, char *argv[])
{
  // all copy elided
  A a = generate_a();
  std::cout << "---------------------------" << std::endl;
  // parameter will be created with cctor, return copy is elided, but b will be
  // inited with cctor
  A b = return_param(a);
  std::cout << "---------------------------" << std::endl;
  // b will still need to be inited with cctor
  A c = return_param(generate_a());
  std::cout << "---------------------------" << std::endl;
  // all copy elided
  A d = return_int_param(5);

  return 0;
}
