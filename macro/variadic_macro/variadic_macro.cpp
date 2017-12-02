#include <iostream>

// by default you can use __VA_ARGS__ as argument list
#define eprintf0(...) printf(__VA_ARGS__)

// custom variable argument name
#define eprintf1(args...) printf(args)

// you can combine named argument and variable argument, but it's not
// recommanded, you will need at least 1 argument after format to make this
// macro work.(GUN CPP and c++2a fixed this problem)
#define eprintf2(format, args...) printf(format, args)

int main(int argc, char *argv[]) {
  eprintf0("%d+%d=%d\n", 1, 1, 2);
  eprintf1("%d+%d=%d\n", 1, 2, 3);
  eprintf2("%d+%d=%d\n", 1, 3, 4);
  // eprintf2("1+4=5");  //need to feed args
  return 0;
}
