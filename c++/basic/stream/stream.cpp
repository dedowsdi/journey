/*
 *  ios_base:
 *    openmode
 *    fmtflags
 *    iostate:
 *      goodbit (0)
 *      badbit
 *      failbit
 *      eofbit
 *
 *  fmtflats and precision are sticky, width is one shot only.
 *
 *  basic_ios:
 *    good() = iostate == 0
 *    bad() = iostate & badbit
 *    fail() = iostate & (failbit | badbit)
 *    eof() = isotate & eofbit
 *    operator bool() = !fail()
 *    operator !() = !operator bool()
 */

#include <iostream>
#include <iomanip>

/*
 * precision is not part of std::ios::fmtflag, it needs to be handled together
 * with fmtflag.
 */
void restoreIos(){
  std::ios_base::fmtflags flags = std::cout.flags();
  unsigned precision = std::cout.precision(); 
  std::cout << 0.123456f << std::endl;
  std::cout << std::setprecision(2) << 0.123456f << std::endl;
  std::cout << 0.234567f << std::endl;
  std::cout.flags(flags);
  std::cout.precision(precision);
  std::cout << 0.123456f << std::endl;
}


int main(int argc, char *argv[])
{
  restoreIos();
  
  return 0;
}
