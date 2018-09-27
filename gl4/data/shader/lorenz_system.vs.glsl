#version 330 core

in vec4 vertex;
in vec4 color;

out VS_OUT
{
  vec4 color;
}vo;

uniform mat4 mvp_mat;

void main(void)
{
  vo.color = color;
  gl_Position = mvp_mat * vertex;
}
