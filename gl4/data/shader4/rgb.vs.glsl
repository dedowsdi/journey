#version 430 core

in vec4 vertex;
in vec4 color;

uniform mat4 mvp_mat;

out vs_out{
  flat vec4 color;
}vo;

void main(void)
{
  gl_Position = mvp_mat * vertex;
  vo.color = color;
}
