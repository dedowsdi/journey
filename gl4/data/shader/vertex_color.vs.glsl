#version 330 core

in vec4 vertex;
in vec3 color;

uniform mat4 mvp_mat;

out vs_out
{
 vec3 color;
}vo;

void main(void)
{
  gl_Position = mvp_mat * vertex;
  vo.color = color;
}
