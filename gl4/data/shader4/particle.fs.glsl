#version 330 core

in vs_out{
  vec4 color;
  vec2 texcoord;
}fi;

uniform sampler2D diffuse_map;

out vec4 frag_color;

void main(void)
{
  frag_color = texture(diffuse_map, fi.texcoord) * fi.color;
}
