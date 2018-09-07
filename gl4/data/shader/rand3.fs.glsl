#version 430 core

#define PI 3.1415926535897932384626433832795

uniform vec2 resolution;
uniform vec2 mouse;
uniform float time;

out vec4 frag_color;

void main(void)
{
  vec2 st = gl_FragCoord.xy / resolution;
  
}
