#version 330 core

in VS_OUT{
  vec2 texcoord;
}fs_in;

uniform sampler2D diffuseMap;

out vec4 fragColor;

void main(void)
{
  fragColor = texture(diffuseMap, fs_in.texcoord);
}
