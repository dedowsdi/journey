#ifndef STRINGUTIL_H
#define STRINGUTIL_H
#include <string>
#include "glm.h"

namespace zxd {
class string_util {
public:
  static std::string tail(const std::string& path);
  static std::string basename(const std::string& filename);
};
}

#endif /* STRINGUTIL_H */
