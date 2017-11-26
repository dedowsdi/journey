#ifndef STRINGUTIL_H
#define STRINGUTIL_H

#include <string>
#include <sstream>
#include <osg/Notify>

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
};
}

#endif /* STRINGUTIL_H */
