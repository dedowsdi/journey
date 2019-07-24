#ifndef GL4_GLC_COLOR_H
#define GL4_GLC_COLOR_H

#define GLM_FORCE_SWIZZLE

#include "glm.h"

namespace zxd
{
using namespace glm;

  vec3 rgb2hsb(const vec3& c);

  vec3 hsb2rgb(const vec3& c);

}


#endif /* GL4_GLC_COLOR_H */
