#include <iostream>
#include <random>


int main(int argc, char *argv[])
{
  std::random_device rd; // used to obtain a seed
  std::mt19937 engine0(rd());
  std::uniform_real_distribution<> r0(0.0f, 1.0f);

  std::cout << r0(engine0) << std::endl;

  // engine with fixed seed, sometimes you need this to recreate random sequence
  std::mt19937 engine1(100);
  std::cout << r0(engine1) << std::endl;
  
  return 0;
}
