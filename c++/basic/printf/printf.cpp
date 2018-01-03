#include <iostream>

void printString() {
  const char* s = "123456789";
  printf("%.5s\n", s);
  printf("%.*s\n", -1, s);  // specific length by extra integer value
  printf("%.*s\n", 5, s);  // specific length by extra integer value
}

int main(int argc, char* argv[]) {
  printString();
  return 0;
}
