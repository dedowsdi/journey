#version 430 core

layout(location = 0) in vec4 vertex;
layout(location = 1) in vec2 texcoord;

layout(location = 0) uniform mat4 mvp_mat;

out vs_out
{
 vec2 texcoord;
}vo;

void main(void)
{
  gl_Position = mvp_mat * vertex;
  vo.texcoord = texcoord;
}
