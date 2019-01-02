#version 430 core

#define PI 3.1415926535897932384626433832795

uniform vec2 resolution;
out vec4 frag_color;

vec2 rect_test(vec2 st)
{
  return st * (1 - st);
}

// create rect in screen center.
float rect(vec2 st, vec2 size, float smoothedge)
{
  vec2 t0 = rect_test(st);
  vec2 t1 = rect_test((1-size)/2);
  //vec2 uv = step(t0, t1);
  vec2 uv = smoothstep(t1, t1+smoothedge, t0);
  return uv.x * uv.y;
}

mat2 rotate2d(float a)
{
  float c = cos(a);
  float s = sin(a);
  return mat2(c,s,-s,c);
}

vec2 tile(vec2 st, int x, int y)
{
  st.x *= x;
  st.y *= y;
  return fract(st);
}

void main(void)
{
  vec2 st = gl_FragCoord.xy / resolution;

  st = tile(st, 5, 5);
  // rotate around box center
  st -= vec2(0.5);
  st = rotate2d(PI * 0.25) * st;
  st += vec2(0.5);

  vec3 white_color = vec3(1.0);
  vec3 bg_color = vec3(0);

  // 0.707 = 0.5 * sqrt(2)
  vec3 color = mix(bg_color, white_color, rect(st, vec2(0.707107), 0.0001));
  frag_color = vec4(color, 1);
}
