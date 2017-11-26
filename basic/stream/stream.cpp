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
 *    operator bool() = iostate == 0 || iostate == eofbit  or !fail()
 *    operator !() = !operator bool()
 */

#include <iostream>

int main(int argc, char *argv[])
{
  
  return 0;
}
