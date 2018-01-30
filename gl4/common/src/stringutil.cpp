#include "stringutil.h"

namespace zxd {

//--------------------------------------------------------------------
std::string string_util::tail(const std::string& path) {
  std::string::size_type pos = path.find_last_of("/\\");
  return pos == std::string::npos ? path : path.substr(pos + 1);
}

//--------------------------------------------------------------------
std::string string_util::basename(const std::string& filename) {
  std::string::size_type pos = filename.find_last_of(".");
  return pos == std::string::npos ? filename : filename.substr(0, pos);
}
}
