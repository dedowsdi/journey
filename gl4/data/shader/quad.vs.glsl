#version 430 core

in vec4 vertex;
in vec2 texcoord;

out VS_OUT
{
 vec2 texcoord;
}vs_out;

void main(void)
{
  gl_Position = vertex;
  vs_out.texcoord = texcoord;
}
