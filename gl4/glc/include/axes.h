#ifndef GL_GLC_AXES_H
#define GL_GLC_AXES_H

#include "geometry.h"

namespace zxd
{

class axes : public common_geometry
{

private:
  vertex_build build_vertices() override;
  array_uptr build_colors(const array& vertices) override;
};

}

#endif /* GL_GLC_AXES_H */
