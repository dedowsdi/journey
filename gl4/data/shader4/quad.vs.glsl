#version 430 core

in vec4 vertex;
in vec2 texcoord;

out vs_out
{
 vec2 texcoord;
}vo;

void main(void)
{
  gl_Position = vertex;
  vo.texcoord = texcoord;
}
