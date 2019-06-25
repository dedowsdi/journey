#ifndef GL_GLC_AXES_H
#define GL_GLC_AXES_H

#include "geometry.h"

namespace zxd
{
class axes : public geometry_base
  {
public:
  virtual void build_vertex();
  virtual void build_color();

};
}

#endif /* GL_GLC_AXES_H */
