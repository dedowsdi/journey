/*
 * http://www.fmddlmyy.cn/text6.html
 *
 * ACCII  7 bits
 * extended ASCII, 0x80 to 0xff is not fixed, don't go there
 * UNICODE  "Universal Multiple-Octet Coded Character Set"，short as UCS
 * UCS defined how to encode, but don't care about transform and storage
 * UCS-2  2 bytes unicode
 * UCS-4  4 bytes unicode
 *  BMP   0x0000****
 * UTF   UCS Transformation Format
 *
 * UCS-2 and UTF-8
 *
 *   UCS-2            UTF-8
 *   0x0000-0x007f    0b0*******
 *   0x0080-0x07ff    0b110***** 10******
 *   0x08ff-0xffff    0b1110**** 10****** 10******
 *
 * example:
 *   binary of 6C49 (汉) is 0110 1100 0100 1001, as 08ff < 6c49 < 0xffff, it's
 *   three bytes in utf-8:
 *      11100110 10110001 10001001
 *
 * utf-16 is exactly the same as ucs-2 in bmp
 *
 * BOM  byte order mask, appears in the beginning of a string
 * utf16:
 *   FEFF big endian. FEFF is some "ZERO WIDTH NO-BREAK SPACE" character which
 *   FFFE little endian
 *
 * there has no BOM for utf8, but it can be used to describe utf8 encoding:
 *   FF BB BF
 *
 *  windows support only utf-16
 *  linux works well with utf-8
 */
#include <iostream>
#include <string>

// the number of characters in a multibyte string is the sum of mblen()'s
// note: the simpler approach is std::mbstowcs(NULL, s.c_str(), s.size())
std::size_t strlen_mb(const std::string& s)
{
  std::size_t result = 0;
  const char* ptr = s.data();
  const char* end = ptr + s.size();
  std::mblen(NULL, 0); // reset the conversion state
  while (ptr < end) {
    int next = std::mblen(ptr, end-ptr);
    if (next == -1) {
      throw std::runtime_error("strlen_mb(): conversion error");
    }
    ptr += next;
    ++result;
  }
  return result;
}

int main(int argc, char *argv[])
{
  //std::string s = u8"z\u00df\u6c34\U0001f34c";
  std::string s = u8"你好啊";
  std::cout << s << std::endl; // crap in windows

  std::setlocale(LC_ALL, "en_US.utf8");
  std::cout << strlen_mb(s) << std::endl;
  std::cout << std::mbstowcs(nullptr, s.data(), s.size()) << std::endl;
  
  return 0;
}
