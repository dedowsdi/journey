#version 330 core

in vec4 vertex;
in vec4 color;
in float size; // size, 0, 0, 0
uniform mat4 mvp_mat;

out VS_OUT
{
  vec4 color;
  float size;
}vo;

void main(void)
{
  gl_Position = mvp_mat * vertex;
  vo.color = color;
  vo.size = size;
  gl_PointSize = size;
}
