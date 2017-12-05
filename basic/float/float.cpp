#include <iostream>
#include <iomanip>
#include <climits>
#include <sstream>
#include <algorithm>
#include "string_util.h"

void printFloat() {
  std::cout << "value | sign | exponent | significand bits "
            << std::endl;
  std::vector<float> numbers({2, 1, 0, 0.5, 0.25, 0.75, -0.625});

  std::ios_base::fmtflags flags = std::cout.flags();
  std::cout << std::left;
  std::for_each(
    numbers.begin(), numbers.end(), [&](decltype(*numbers.begin()) v) {
      std::string s = zxd::StringUtil::toBinaryString(v);
      std::cout << std::setw(8) << v;
      std::cout << std::setw(7) << s[0];
      std::cout << std::setw(11) << s.substr(1, 8);
      std::cout << s.substr(9) << std::endl;
    });
  std::cout << "float = (-1)^signed * (1.significand_b)^(expoinent - 127)" << std::endl;
  std::cout.flags(flags);
}

int main(int argc, char* argv[]) {
  printFloat();
  return 0;
}
