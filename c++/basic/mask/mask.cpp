#include <iostream>
#include <iomanip>
#include <bitset>
#include <algorithm>

int main(int argc, char *argv[]) {
  unsigned int number = 0b11001100;

  std::cout << "use number&x to get number%(x+1), x must be power of 2 - 1"
            << std::endl;
  std::cout << std::bitset<8>(13) << " & " << std::bitset<8>(7) << " = "
            << std::bitset<8>(13 & 0x7) << std::endl
            << std::endl;

  std::cout << "use number&x to check if number/x is odd, x must be power of 2"
            << std::endl;
  std::cout << std::bitset<8>(13) << " & " << std::bitset<8>(8) << " = "
            << std::bitset<8>(13 & 0x8) << std::endl
            << std::endl;

  std::cout << "use number |= 1 << n to set a bit to 1" << std::endl;
  std::cout << std::bitset<8>(number) << " | " << std::bitset<8>(1 << 1)
            << " = " << std::bitset<8>(number | 1 << 1) << std::endl
            << std::endl;

  std::cout << "use number &= ~(1<<n) to clear a bit to 0" << std::endl;
  std::cout << std::bitset<8>(number) << " & " << std::bitset<8>(~(1 << 2))
            << " = " << std::bitset<8>(number & ~(1 << 2)) << std::endl
            << std::endl;

  std::cout << "use number ^= 1<<n to toggle bit" << std::endl;
  std::cout << std::bitset<8>(number) << " ^ " << std::bitset<8>(1 << 1)
            << " = " << std::bitset<8>(number ^ (1 << 1)) << std::endl;
  std::cout << std::bitset<8>(number) << " ^ " << std::bitset<8>(1 << 1)
            << " ^ " << std::bitset<8>(1 << 1) << " = "
            << std::bitset<8>(number ^ (1 << 1) ^ (1 << 1)) << std::endl
            << std::endl;

  std::cout << "use number ^= 0 to do nothing" << std::endl;
  std::cout << std::bitset<8>(number) << " ^ " << std::bitset<8>(0) << " = "
            << std::bitset<8>(number ^ (0)) << std::endl
            << std::endl;

  std::cout << "use (number >> n) & 1 to check a bit" << std::endl;
  std::cout << "(" << std::bitset<8>(number)
            << " >> 2) & 1 = " << std::bitset<8>((number >> 2) & 1) << std::endl
            << std::endl;

  std::cout << "use number ^= (-x ^ number) & (1 << n) to change nth bit to x"
            << std::endl;
  std::cout << std::bitset<8>(number) << " ^ ((" << std::bitset<8>(0) << " ^ "
            << std::bitset<8>(number) << ") & " << std::bitset<8>(1 << 2)
            << ") = " << std::bitset<8>(number ^ ((0 ^ number) & (1 << 2)))
            << std::endl;
  std::cout << std::bitset<8>(number) << " ^ ((" << std::bitset<8>(-1) << " ^ "
            << std::bitset<8>(number) << ") & " << std::bitset<8>(1 << 1)
            << ") = " << std::bitset<8>(number ^ ((-1 ^ number) & (1 << 1)))
            << std::endl
            << std::endl;

  return 0;
}
