#version 330 core

out vec4 frag_color;
uniform vec4 color = vec4(1);

void main(void)
{
  vec2 st = gl_PointCoord - vec2(0.5);
  float pct = 1 - smoothstep(0.24, 0.25, dot(st, st));
  if(pct == 0)
    discard;

  frag_color = color;
}
