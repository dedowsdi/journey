#version 430 core

#define PI 3.1415926535897932384626433832795

uniform vec2 resolution;
out vec4 frag_color;

float random(vec2 st)
{
  // you try dot(st, st), very funny result
  return fract(sin(dot(st, vec2(12.9898,78.233))) * 43758.5453123);
}

void main(void)
{
  vec2 st = gl_FragCoord.xy / resolution;
  float rnd = random(st);
  frag_color = vec4(vec3(rnd), 1);
}
