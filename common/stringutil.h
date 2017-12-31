#ifndef STRINGUTIL_H
#define STRINGUTIL_H
#include <string>

namespace zxd {
class StringUtil {
public:
  static std::string tail(const std::string& path);
  static std::string basename(const std::string& filename);
};
}

#endif /* STRINGUTIL_H */
