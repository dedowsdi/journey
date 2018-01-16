/*
 *sizeof(std::array) works just like sizeof(built in array)
 */

#include <iostream>
#include <array>

int main(int argc, char *argv[]) {
  std::array<int, 4> ia{1, 2, 3, 4};

  std::cout << "sizeof std::array<int, 4> is " << sizeof(ia) << std::endl;

  return 0;
}
