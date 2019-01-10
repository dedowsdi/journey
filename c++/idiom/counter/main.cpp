#include <iostream>

#include "derived0.h"
#include "derived1.h"

int main(int argc, char *argv[])
{
  try {
    derived0 a0;
    std::cout << "there are " << derived0::object_count() << " derived0" << std::endl;
    derived0 a1;
    std::cout << "there are " << derived0::object_count() << " derived0" << std::endl;
    derived0 a2;
    std::cout << "there are " << derived0::object_count() << " derived0" << std::endl;
  
  }catch(const derived0::too_many_objects& e) {
    std::cout << "too many objects" << std::endl;
  }
  return 0;
}
