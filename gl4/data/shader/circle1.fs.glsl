#version 430 core

uniform vec2 resolution;
out vec4 frag_color;

void main(void)
{
  vec2 st = gl_FragCoord.xy/resolution;

  // shift center to 0.5, 0.5, then shrink to 50%;
  st = st*2 - 1;

  float d = length(abs(st) - 0.5);

  //frag_color = vec4(vec3(fract(d * 10)), 1);
  //frag_color = vec4(vec3(step(.25, d)), 1);
  //frag_color = vec4(vec3(step(.3,d) * step(d,.4)),1.0);
  frag_color = vec4(vec3(smoothstep(.3, .32, d) * smoothstep(d, d+0.02, .4)),1.0);
}
