#version 430 core

in vs_out
{
  vec2 texcoord;
}fi;

layout(location = 4) uniform vec4 color0 = vec4(1);
layout(location = 5) uniform vec4 color1 = vec4(0, 0, 0.5, 1);

out vec4 frag_color;

void main(void)
{
  float f = abs(fi.texcoord.x - 0.5) * 2.0f;
  frag_color = mix(color0, color1, f);
}
