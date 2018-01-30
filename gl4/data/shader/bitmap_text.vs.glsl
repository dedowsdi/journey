#version 430 core

in vec4 vertex;

uniform mat4 mvp_mat;

out vs_out{
  vec2 texcoord;
} vo;

void main(void)
{
  gl_Position = mvp_mat * vec4(vertex.xy, 0, 1);
  vo.texcoord = vertex.zw;
}
