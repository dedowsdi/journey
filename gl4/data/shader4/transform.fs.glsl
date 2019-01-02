#version 430 core

#define PI 3.1415926

/*
 * if you transform x in f(x), you are transforming the frame, it's the inverse
 * of transforming point, you can pretend to transform point, then apply inverse
 *
 */

uniform vec2 resolution;
out vec4 frag_color;

mat2 rotate2d(float a)
{
  float c = cos(a);
  float s = sin(a);
  return mat2(c,s,-s,c);
}

//mat2 scale2d(vec2 s)
//{
  //return mat2(s.x, 0, s.y, 0);
//}

// there is no need to create translate2d or scale2d, you can just add or
// scale st. 

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

void main(void)
{
  vec2 st = gl_FragCoord.xy / resolution;

  // move to origin, scale, rotate, move back
  st -= 0.5;
  st *= 0.5;
  st = rotate2d(-PI * 0.25) * st;
  st += 0.5;

  float f = cross(st, 0.5);
  frag_color = vec4(f);
}
