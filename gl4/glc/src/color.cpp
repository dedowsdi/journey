#include "color.h"
#include <glm/glm.hpp>

namespace zxd
{

//--------------------------------------------------------------------
vec3 rgb2hsb(const vec3& c)
{
  vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
  // vec4 p = mix(vec4(c.bg(), K.wz()), vec4(c.gb(), K.xy()), c.b < c.g ? 0.0f : 1.0f);
  // vec4 q = mix(vec4(p.xyw(), c.r), vec4(c.r, p.yzx()), p.x < c.r ? 0.0f : 1.0f);
  vec4 p = mix(vec4(c.b, c.g, K.w, K.z), vec4(c.g, c.b, K.x, K.y), c.b < c.g ? 0.0f : 1.0f);
  vec4 q = mix(vec4(p.x, p.y, p.w, c.r), vec4(c.r, p.y, p.z, p.x), p.x < c.r ? 0.0f : 1.0f);
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

//--------------------------------------------------------------------
float rgb2luminance(const vec3& c, const vec3& weight/* = vec3(0.3086, 0.6094, 0.0820)*/)
{
  return glm::dot(c, weight);
}

//--------------------------------------------------------------------
GLfloat color_difference2(const vec3& c0, const vec3& c1)
{
  // https://en.wikipedia.org/wiki/Color_difference
  GLfloat R = c1.r - c0.r;
  GLfloat G = c1.g - c0.g;
  GLfloat B = c1.b - c0.b;
  return R*R*2 + G*G*4 + B*B*3;
}

//--------------------------------------------------------------------
GLfloat color_difference_256_2(const vec3& c0, const vec3& c1)
{
  // something is wrong, it doesn't work.
  u8vec3 color0 = u8vec3(c0.x * 255, c0.y * 255, c0.z * 255);
  u8vec3 color1 = u8vec3(c1.x * 255, c1.y * 255, c1.z * 255);
  GLfloat r = (color0.r + color1.r) * 0.5f;
  GLfloat R = color0.r - color1.r;
  GLfloat G = color0.g - color1.g;
  GLfloat B = color0.b - color1.b;
  GLfloat R2 = R*R;
  GLfloat B2 = B*B;
  return 2*R2 + 4*G*G + 3*B2 + r * (R2-B2)/256.0;
  //return (2+r/256.0) * R2 + 4*G*G + (2+(255-r)/256)*B2;
}

}
