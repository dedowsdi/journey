#version 330 core

in vec4 vertex;
in vec2 texcoord;

out VS_OUT
{
  vec2 texcoord;
}vo;

void main(void)
{
  gl_Position = vertex;
  vo.texcoord = texcoord;
}
