#version 330 core

in VS_OUT
{
  vec4 color;
  float size;
}fi;

out vec4 frag_color;

float circle(vec2 st)
{
  float l2 = dot(st, st);
  return 1 - smoothstep(0.25 - 0.1, 0.25, l2);
}

void main(void)
{
  vec2 st = gl_PointCoord.xy - vec2(0.5);
  frag_color = fi.color;
  frag_color.a *= circle(st);
}
