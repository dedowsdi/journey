#ifndef GL4_GLC_COLOR_H
#define GL4_GLC_COLOR_H

#include "glm.h"

namespace zxd
{
using namespace glm;

  vec3 rgb2hsb(const vec3& c);

  //  Function from Iñigo Quiles
  //  https://www.shadertoy.com/view/MsS3Wc
  vec3 hsb2rgb(const vec3& c);

  float rgb2luminance(const vec3& c, const vec3& weight = vec3(0.3086, 0.6094, 0.0820));

  GLfloat color_difference2(const vec3& c0, const vec3& c1);

  GLfloat color_difference_256_2(const vec3& c0, const vec3& c1);
}


#endif /* GL4_GLC_COLOR_H */
