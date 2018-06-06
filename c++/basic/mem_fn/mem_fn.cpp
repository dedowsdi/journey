#include <functional>
#include <iostream>

struct A{
  int foo(int i){
    return i;
  }
};

int main(int argc, char *argv[])
{

  A a;
  auto foo = std::mem_fn(&A::foo);
  std::cout << foo(a,5) << std::endl;
  
  return 0;
}
