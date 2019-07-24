#include "color.h"

namespace zxd
{

//--------------------------------------------------------------------
vec3 rgb2hsb(const vec3& c)
{
  vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
  vec4 p = mix(vec4(c.bg(), K.wz()), vec4(c.gb(), K.xy()), c.b < c.g ? 0.0f : 1.0f);
  vec4 q = mix(vec4(p.xyw(), c.r), vec4(c.r, p.yzx()), p.x < c.r ? 0.0f : 1.0f);
  float d = q.x - min(q.w, q.y);
  float e = 1.0e-10;
  return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

//--------------------------------------------------------------------
vec3 hsb2rgb(const vec3& c)
{
  vec3 rgb = clamp(abs(mod(c.x*6.0f+vec3(0.0,4.0,2.0), 6.0f)-3.0f)-1.0f, 0.0f, 1.0f);
  rgb = rgb*rgb*(3.0f-2.0f*rgb);
  return c.z * mix(vec3(1.0), rgb, c.y);
}

}
