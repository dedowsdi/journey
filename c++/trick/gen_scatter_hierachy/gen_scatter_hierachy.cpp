#include "gen_scatter_hierachy.h"
#include <iostream>

template<typename T>
struct holder{
  int value;
};

typedef mp::gen_scatter_hierachy<TYPE_LIST3(int, bool, double), holder> info;


int main(int argc, char *argv[])
{
  info giant;
  std::cout << mp::field<int>(giant).value << std::endl;
  std::cout << mp::field<bool>(giant).value << std::endl;
  std::cout << mp::field<double>(giant).value << std::endl;
  std::cout << mp::field<0>(giant).value << std::endl;
  std::cout << mp::field<1>(giant).value << std::endl;
  std::cout << mp::field<2>(giant).value << std::endl;
  
  return 0;
}
