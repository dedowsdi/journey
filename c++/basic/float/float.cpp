#include <iostream>
#include <iomanip>
#include <climits>
#include <sstream>
#include <algorithm>
#include "string_util.h"

void printFloat() {
  std::cout << std::left;
  std::cout << std::setw(10) << "value"
            << std::setw(10) << "sign"
            << std::setw(10) << "exponent"
            << "significant bits (mantissa withouth the leading 1.)"
            << std::endl;

  std::vector<float> numbers({2.0f, 1.0f, 0.0f, 0.5f, 0.25f, 0.75f, -0.625f, 1000.43f, 0.43f, 1000.43f - 1000.0f});

  std::ios_base::fmtflags flags = std::cout.flags();
  std::cout << std::left;
  std::for_each(
    numbers.begin(), numbers.end(), [&](decltype(*numbers.begin()) v) {
      std::string s = zxd::StringUtil::toBinaryString(v);
      std::cout << std::setw(10) << v;
      std::cout << std::setw(10) << s[0];
      std::cout << std::setw(10) << s.substr(1, 8);
      std::cout << s.substr(9) << std::endl;
    });
  std::cout << "float = (-1)^signed * 2^(exponent - 127) * (1.significand_b)\n";
  std::cout << "You can also apply shift 2^(exponent-127) times to mantissa:\n";
  std::cout << "(-1)^signed * 1.significand_b << (2^(exponent - 127))\n";
  std::cout.flags(flags);
}

int main(int argc, char* argv[]) {
  printFloat();
  return 0;
}
