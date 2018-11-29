#include <iostream>

class A
{
private:
  // this is a constant declaration, as long as you don't take address of
  // sSize0, although you can still use them
  static const int sSize0 = 8;
  int ia0[sSize0]; // works in compile time

  // sometimes above trick doesn't work, you can use enum hack instead
  enum
  {
    ArraySize = 6
  };

  int ia1[ArraySize]; // enum can be used as compile time constant, lots of meta programming use this trick

};

int main(int argc, char *argv[])
{
  A a;
  return 0;
}
