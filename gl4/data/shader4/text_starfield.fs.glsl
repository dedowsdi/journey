#version 430 core

uniform sampler2D diffuse_map;
uniform vec4 color;
in vs_out
{
  vec2 texcoord;
}fi;

out vec4 frag_color;

void main(void)
{
  frag_color = color * vec4(1, 1, 1, texture(diffuse_map, fi.texcoord).r);
}
