#include <iostream>
#include <iomanip>


int main(int argc, char *argv[])
{
  // convert between signed and unsigned of the same type does nothing to the
  // underlying binary bits.
  std::cout.setf(std::ios_base::hex, std::ios_base::basefield);
  std::cout << -1 << std::endl;
  std::cout << static_cast<unsigned int>(-1) << std::endl;
  std::cout.setf(std::ios_base::dec, std::ios_base::basefield);

  // if you promote singed integer type to int or unsigned int, sign bit will be
  // used to pad the leadding bytes
  // if you promote unsigned integer type to int or unsigned int, 0 will be used
  // to pad the leading bytes
  //
  // so if you want to convert char to short to unsigned int while reserve the
  // underlying binary code, make sure you double convert it.
  char c0 = '\xf0'; // signed
  std::cout << static_cast<unsigned int>(c0) << std::endl; // signed to unsigned,
  std::cout << static_cast<int>(c0) << std::endl; // interger promotion, reserve value
  std::cout << static_cast<int>(static_cast<unsigned char>(c0)) << std::endl;
  std::cout << static_cast<unsigned int>(static_cast<unsigned char>(c0)) << std::endl;

  char c1 = '\x0f';
  std::cout << static_cast<unsigned int>(c1) << std::endl;
  std::cout << static_cast<int>(c1) << std::endl; // integer promotion, reserve value
  std::cout << static_cast<int>(static_cast<unsigned char>(c1)) << std::endl;
  std::cout << static_cast<unsigned int>(static_cast<unsigned char>(c1)) << std::endl;

  short s0 = 0xffff;
  std::cout << static_cast<unsigned int>(s0) << std::endl;
  std::cout << static_cast<int>(s0) << std::endl; // pad 1
  std::cout << static_cast<int>(static_cast<unsigned short>(s0)) << std::endl;
  std::cout << static_cast<unsigned int>(static_cast<unsigned short>(s0)) << std::endl;

  // 'a' and 'b' will be promoted to int before +, the same thing happens to
  // unsigned char, short, unsigned short.
  char c = 'a' + 'b';

  return 0;
}
