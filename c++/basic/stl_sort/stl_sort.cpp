#include <iostream>
#include <algorithm>
#include "timer.h"

bool print = false;
int numData = 50000;
#define ARRAY_SIZE 100 

int ctorCount = 0;
int cctorCount = 0;
int dtorCount = 0;
int assignCount = 0;

struct A
{
  int x = rand();
  int data[ARRAY_SIZE];
  A()
  {
    if(print)
      std::cout << "A()" << ctorCount++ << std::endl;
  }

  A(const A& a):
    x(a.x)
  {
    for (int i = 0; i < ARRAY_SIZE; ++i) {
      data[i] = a.data[i];
    }
    if(print)
      std::cout << "A(const A& a)" << cctorCount++ << std::endl;
  }

  A& operator= (const A& a)
  {
    x = a.x;
    for (int i = 0; i < ARRAY_SIZE; ++i) {
      data[i] = a.data[i];
    }
    if(print)
      std::cout << "operator=(const A& a)" << assignCount++ << std::endl;
    return *this;
  }
};

bool operator< (const A& lhs, const A& rhs)
{
  return lhs.x < rhs.x;
}


int main(int argc, char *argv[])
{
  srand(time(0));
  std::vector<A> va;
  va.resize(numData);
  //print = true;

  zxd::Timer timer;
  std::sort(va.begin(), va.end());
  std::cout << "sort object directly finished in " << timer.miliseconds() << "ms" << std::endl;

  std::vector<A*> vp;
  for (int i = 0; i < numData; ++i) {
    vp.push_back(new A());
  }


  timer.reset();
  std::sort(vp.begin(), vp.end(), 
      [](const A* a0, const A* a1)->bool
      {
        return a0->x < a1->x;
      });
  std::cout << "sort pointer finished in " << timer.miliseconds() << "ms" << std::endl;


  for (int i = 0; i < numData; ++i) {
    delete vp[i];
  }
  
  return 0;
}
