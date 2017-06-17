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
  static inline T parse(const std::string& val) {

    std::stringstream ss(val);
    T t;
    ss >> t;

    if (ss.fail() || !ss.eof())
      OSG_FATAL << val << " is not paseable" << std::endl;

    return t;
  }

  static inline char parseChar(const std::string& val) {
    return parse<char>(val);
  }
  static inline short parseShort(const std::string& val) {
    return parse<short>(val);
  }
  static inline int parseInt(const std::string& val) { return parse<int>(val); }
  static inline long parseLong(const std::string& val) {
    return parse<long>(val);
  }
  static inline size_t parseSize_t(const std::string& val) {
    return parse<size_t>(val);
  }
  static inline unsigned int parseUInt(const std::string& val) {
    return parse<unsigned int>(val);
  }
  static inline unsigned short parseUShort(const std::string& val) {
    return parse<unsigned short>(val);
  }
  static inline float parseFloat(const std::string& val) {
    return parse<float>(val);
  }
  static inline double parseDouble(const std::string& val) {
    return parse<double>(val);
  }
};
}

#endif /* STRINGUTIL_H */
