#version 430 core

#define PI 3.1415926

uniform vec2 resolution;

out vec4 frag_color;

float rect(vec2 st, vec2 size)
{
  vec2 half_size = vec2(0.5) - size * 0.5;
  vec2 uv = smoothstep(half_size, half_size + 0.001,  st);
  uv *= smoothstep(half_size, half_size+0.001,  vec2(1)-st);
  return uv.x * uv.y;
}

float cross(vec2 st, float size)
{
  float a = rect(st, vec2(size, size/4));
  float b = rect(st, vec2(size/4, size));
  return a+b;
}

mat2 rotate2d(float a){
  return mat2(cos(a),-sin(a),
              sin(a),cos(a));
}

void main(void)
{
  vec2 st = gl_FragCoord.xy / resolution;
  st = rotate2d(PI * 0.25) * st;
  float f = cross(st, 0.25);
  frag_color = vec4(vec3(f), 1);
}
