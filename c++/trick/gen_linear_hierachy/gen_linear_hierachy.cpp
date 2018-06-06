/*
 * ??????? what's the point of linear hierachy???
 */
#include <iostream>
#include "gen_linear_hierachy.h"

struct A{
  void foo(){
    std::cout << "afoo" << std::endl;
  }
};

struct B{
  void foo(){
    std::cout << "bfoo" << std::endl;
  }
};

struct C{
  void foo(){
    std::cout << "cfoo" << std::endl;
  }
};

template<typename T, typename Base>
struct Holder{
  virtual void foo(T& t){
    t.foo();
  }
};

typedef mp::gen_linear_hierachy< TYPE_LIST3(C, B, A), Holder> ABC;

int main(int argc, char *argv[])
{
  ABC abc;
  
  return 0;
}


