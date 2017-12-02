#include <iostream>

#define xstr(s) str(s)
#define str(s) #s
#define foo 4

int main(int argc, char *argv[]) {
  std::cout << str(foo) << std::endl;   // print foo
  std::cout << xstr(foo) << std::endl;  // print expanded foo
  return 0;
}
