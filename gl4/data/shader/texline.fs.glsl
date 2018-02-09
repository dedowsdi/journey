#version 330 core

in vs_out{
 float texcoord;
} fi;

uniform vec4 color = vec4(1.0);
uniform sampler1D diffuse_map;

out vec4 fragColor;

void main(void)
{
  fragColor = texture(diffuse_map, fi.texcoord);
}
