#version 330 core
attribute vec4 vertex;

out vec2 texcoord;

uniform mat4 mvp_mat;

void main(void)
{
  gl_Position = mvp_mat * vec4(vertex.xy, 0, 1);
  texcoord = vertex.zw;
}
