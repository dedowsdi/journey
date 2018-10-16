#version 330 core

uniform sampler2D diffuse_map;

in VS_OUT
{
  vec2 texcoord;
}fi;

out vec4 frag_color;

void main(void)
{
  frag_color = texture(diffuse_map, fi.texcoord);
}
