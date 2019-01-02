#version 330 core

in vec4 vertex;
in vec2 texcoord;

uniform mat4 mvp_mat;

out VS_OUT
{
  vec2 texcoord;
}vo;

void main(void)
{
  vo.texcoord = texcoord;
  gl_Position = mvp_mat * vertex;
}
