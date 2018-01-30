#version 430 core

in vs_out
{
 vec2 texcoord;
}vs_in;

uniform sampler2D quadMap;

out vec4 fragColor;

void main(void)
{
  fragColor = texture(quadMap, vs_in.texcoord);
}
