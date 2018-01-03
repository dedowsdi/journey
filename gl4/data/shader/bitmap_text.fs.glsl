#version 430 core

in VS_OUT{
  vec2 texcoord;
} fs_in;

out vec4 fragColor;

uniform sampler2D fontMap;
uniform vec4 textColor = vec4(1);

void main(void)
{
  vec4 color = vec4(1, 1, 1, texture(fontMap, fs_in.texcoord).r);
  fragColor = textColor * color;
}
