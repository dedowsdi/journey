#version 430 core

#define PI 3.1415926535897932384626433832795


// when n<0.5, only part of curve can be seen, increate smooth edge can migigate
// the problem.

uniform vec2 resolution;
uniform float a;
uniform float b;
uniform float n;

out vec4 frag_color;

float superellipse(vec2 st, float edge)
{
  float theta = atan(st.y, st.x);
  float an = pow(a, n);
  float bn = pow(b, n);
  float r = pow((an*bn)/(pow(abs(cos(theta)/a), n) +  pow(abs(sin(theta)/b), n)), 1/n);

  float d = length(st);

  return smoothstep(r - edge, r, d) * (1 - smoothstep(r, r+edge, d));
}

void main(void)
{
  vec2 st = gl_FragCoord.xy / resolution;
  st = st*2 - 1;

  st *= 1.1; // shrink a little bit

  vec3 fg_color = vec3(1);
  vec3 bg_color = vec3(0);

  vec3 color = mix(bg_color, fg_color, superellipse(st, 0.02));

  frag_color = vec4(color, 1);
}
