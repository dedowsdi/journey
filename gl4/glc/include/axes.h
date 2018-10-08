#ifndef AXES_H
#define AXES_H

#include "geometry.h"

namespace zxd {
class axes : public geometry_base {
public:
  virtual void build_vertex();
  virtual void build_color();

};
}

#endif /* AXES_H */
