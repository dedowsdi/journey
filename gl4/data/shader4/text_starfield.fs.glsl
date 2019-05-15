#version 430 core

uniform sampler2D diffuse_map;
in vs_out
{
  vec2 texcoord;
}fi;

out vec4 frag_color;

void main(void)
{
  vec4 color = vec4(1, 1, 1, texture(diffuse_map, fi.texcoord).r);
  frag_color = color * vec4(1);
}
