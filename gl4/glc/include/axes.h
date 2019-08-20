#ifndef GL_GLC_AXES_H
#define GL_GLC_AXES_H

#include "geometry.h"

namespace zxd
{

class axes : public geometry_base
{

private:
  void build_vertex() override;
  void build_color() override;
};

}

#endif /* GL_GLC_AXES_H */
