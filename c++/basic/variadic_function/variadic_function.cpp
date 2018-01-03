#include <iostream>

void simple_printf(const char* fmt...) {
  va_list args1;  // infomation holder

  va_start(args1, fmt);  // init
  va_end(args1);         // end traversal

  va_list args2;  // another infomation holder
  // make a totally fresh copy, don't care if args1 is ended or not, as long as
  // it's started.
  va_copy(args2, args1);

  while (*fmt != '\0') {
    if (*fmt == 'd') {
      int i = va_arg(args2, int);  // next arguemnt in args2
      std::cout << i << '\n';
    } else if (*fmt == 'c') {
      // note automatic conversion to integral type
      int c = va_arg(args2, int);
      std::cout << static_cast<char>(c) << '\n';
    } else if (*fmt == 'f') {
      double d = va_arg(args2, double);
      std::cout << d << '\n';
    }
    ++fmt;
  }

  va_end(args2);  // clean up
}

int main(int argc, char* argv[]) {
  simple_printf("dcff", 3, 'a', 1.999, 42.5);

  return 0;
}
