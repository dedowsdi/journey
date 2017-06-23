#include "axisconstrainer.h"

namespace zxd {

//------------------------------------------------------------------------------
std::string toString(AxisConstrainer::ConstrainFrame frame) {
  switch (frame) {
    case zxd::AxisConstrainer::CF_LOCAL:
      return "local";
    case zxd::AxisConstrainer::CF_WORLD:
      return "world";
    default:
      return "";
  }
}
}
