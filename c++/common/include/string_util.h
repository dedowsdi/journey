#ifndef STRINGUTIL_H
#define STRINGUTIL_H

#include <string>
#include <sstream>
#include <osg/Notify>
#include <algorithm>
#include <climits>
#include <cstring>
#include <cmath>

namespace zxd {

class StringUtil {
public:
  template <typename T>
  static inline std::string toString(T t) {
    std::stringstream ss;
    ss << t;
    std::string s;
    ss >> s;

    if (ss.fail() || !ss.eof())
      OSG_FATAL << "failed to convert " << t << " to string " << std::endl;

    return s;
  }

  template <typename T>
  static inline T parse(const std::string& val, T falseValue) {
    std::stringstream ss(val);
    T t;
    ss >> t;

    if (ss.fail() || !ss.eof()) return falseValue;
    // OSG_FATAL << val << " is not paseable" << std::endl;

    return t;
  }

  static inline char parseChar(const std::string& val, char falseValue = 'x') {
    return parse<char>(val, falseValue);
  }
  static inline short parseShort(
    const std::string& val, short falseValue = -1) {
    return parse<short>(val, falseValue);
  }
  static inline int parseInt(const std::string& val, int falseValue = -1) {
    return parse<int>(val, falseValue);
  }
  static inline long parseLong(const std::string& val, long falseValue = -1) {
    return parse<long>(val, falseValue);
  }
  static inline size_t parseSize_t(
    const std::string& val, size_t falseValue = -1) {
    return parse<size_t>(val, falseValue);
  }
  static inline unsigned int parseUInt(
    const std::string& val, unsigned falseValue = -1) {
    return parse<unsigned int>(val, falseValue);
  }
  static inline unsigned short parseUShort(
    const std::string& val, unsigned falseValue = -1) {
    return parse<unsigned short>(val, falseValue);
  }
  static inline float parseFloat(
    const std::string& val, float falseValue = -1) {
    return parse<float>(val, falseValue);
  }
  static inline double parseDouble(
    const std::string& val, double falseValue = -1) {
    return parse<double>(val, falseValue);
  }

  // in reverse order of memory
  template <typename T>
  static std::string toBinaryString(const T& t) {
    size_t numBytes = sizeof(t);

    std::stringstream ss;

    const char* p = reinterpret_cast<const char*>(&t);
    for (int i = 0; i < numBytes; ++i) {
      for (int j = 0; j < CHAR_BIT; ++j) {
        ss << ((p[i] >> j) & 1);
      }
    }

    std::string s(ss.str());
    std::reverse(s.begin(), s.end());
    return s;
  }

  template <typename T>
  static T fromBinaryString(const std::string& s) {
    T t;
    std::memset(&t, 0, sizeof(t));

    char* p = reinterpret_cast<char*>(&t);

    int numBytes = std::ceil(s.size() / 8.0);

    // loop starts from least significant byte
    for (int i = 0; i < numBytes; ++i) {
      char c = 0;
      // loop starts from least significant bit(last char of s).
      for (int j = 0, k = s.size() - 8 * i - 1; j < 8 && k >= 0; ++j, --k) {
        if (s[k] == '1') {
          c |= 1 << j;
        }
      }
      p[i] = c;
    }
    return t;
  }
};
}

#endif /* STRINGUTIL_H */
