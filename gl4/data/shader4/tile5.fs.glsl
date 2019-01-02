#version 430 core

// basic truchet pattern
#define PI 3.14159265358979323846

uniform vec2 resolution;
out vec4 frag_color;

vec2 tile(vec2 st, int x, int y)
{
  st.x *= x;
  st.y *= y;
  return fract(st);
}

mat2 rotate2d(float a)
{
  float c = cos(a);
  float s = sin(a);
  return mat2(c,s,-s,c);
}

vec2 rotate_truchet_pattern(vec2 st)
{
  // create 4 mini tile
  st *= 2;

  // setup mini tile index row by row
  float index = step(1.0, mod(st.x, 2)) + step(1.0, mod(st.y, 2))*2;

  // set rotation angle according to tile index
  float angle = 0;

  if(index == 1)
    angle = PI * 0.5;
  else if(index == 2)
    angle = -PI * 0.5;
  else if(index == 3)
    angle = PI;

  st = fract(st);

  st += vec2(-0.5);
  st = rotate2d(-angle) * st;
  st += vec2(0.5);
  return st;
}

void main(void)
{
  vec2 st = gl_FragCoord.xy / resolution;
  st = tile(st, 3, 3);

  st = rotate_truchet_pattern(st);
  frag_color = vec4(step(st.x, st.y));
}
